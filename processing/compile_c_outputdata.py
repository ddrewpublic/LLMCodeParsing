"""compile_c_outputdata.py

Purpose:
    Batch-compile all `.c` source files in a given directory using `gcc`,
    infer common library/linker flags from detected `#include` headers,
    log compile outcomes to a CSV, and sort outputs into success/failed folders.

Key behaviors:
    - Scans each `.c` file for known headers and infers additional flags.
    - Uses `pkg-config` for libraries that publish cflags/libs.
    - Uses direct linker flags for some common libraries (e.g., -lm, -lX11, -ljpeg).
    - Writes a CSV report (`compile_results.csv`) summarizing results.
    - Copies failed sources to a "failed" directory; moves successful `.out` to a "success" directory.
    - Prints colorized status lines to stdout.

Notes:
    - This script assumes `gcc` and (optionally) `pkg-config` are installed and available on PATH.
    - The compile timeout is fixed at 20 seconds per file.
    - No style or code changes have been made—only comments and docstrings added.
"""

import subprocess
import os
import csv
import re
import shutil
import argparse
from datetime import datetime
from colorama import Fore, init
# TODO: add custom logger

# Initialize colorama to automatically reset styling after each print.
init(autoreset=True)

# Output CSV filename for compile results.
CSV_FILE = '../_output/compile_results.csv'


def detect_headers(filepath):
    """Extract a list of headers included by a C source file.

    This function does a very light-weight parse: it simply looks for lines
    that match a basic `#include <...>` or `#include "..."` pattern. It does
    not handle macros, conditional includes, multiline constructs, or unusual
    preprocessor formatting.

    Parameters
    ----------
    filepath : str
        Path to the `.c` file to scan.

    Returns
    -------
    list[str]
        A list of header names as they appear in the `#include` directive
        (e.g., "stdio.h", "X11/Xlib.h", "opencv2/core.hpp").
    """
    headers = []
    # Open in text mode; ignore decoding errors to avoid crashing on odd encodings.
    with open(filepath, 'r', errors='ignore') as f:
        for line in f:
            # Regex captures content between <...> or "..."
            match = re.match(r'\s*#include\s*[<"](\S+)[>"]', line)
            if match:
                headers.append(match.group(1))
    return headers


def get_pkg_flags(pkg_name):
    """Resolve compiler/linker flags via `pkg-config` for a given package.

    For libraries that publish metadata to `pkg-config`, this function gathers
    both `--cflags` and `--libs` and returns them as a flat list of tokens
    (e.g., ['-I/some/include', '-L/some/lib', '-lsomelib']).

    Parameters
    ----------
    pkg_name : str
        The `pkg-config` name (e.g., "opencv4", "sdl2", "json-c").

    Returns
    -------
    list[str]
        A list of flags suitable to extend a `gcc` command invocation.

    Side Effects
    ------------
    Prints a red warning if the package cannot be found by `pkg-config`.
    """
    try:
        # Query compile flags (include dirs, defines, etc.)
        cflags = subprocess.check_output(['pkg-config', '--cflags', pkg_name], text=True).strip().split()
        # Query link flags (library dirs, libraries, etc.)
        libs = subprocess.check_output(['pkg-config', '--libs', pkg_name], text=True).strip().split()
        return cflags + libs
    except subprocess.CalledProcessError:
        print(Fore.RED + f"pkg-config couldn't find {pkg_name}")
        return []


def compile_single_file(filepath, failed_dir, success_dir):
    """Compile a single `.c` file with `gcc`, inferring extra flags from headers.

    Steps performed:
        1. Detect included headers in the file.
        2. Build a `gcc` command with the source path and output `.out` filename.
        3. Infer extra flags:
           - Direct linker flags for known headers without pkg-config (e.g., math/X11/jpeg).
           - `pkg-config`-derived flags for known packages (e.g., opencv4, sdl2, libxml-2.0, json-c, libzip, libcurl).
        4. Execute compilation with a 20-second timeout.
        5. Print a colored success/fail line and any compiler/stdout/stderr output.
        6. Move/copy files according to success state.
        7. Return a result row for CSV logging.

    Parameters
    ----------
    filepath : str
        Path to the `.c` file to compile.
    failed_dir : str
        Directory to which failed source files are copied.
    success_dir : str
        Directory to which successful `.out` executables are moved.

    Returns
    -------
    tuple[dict, bool]
        - dict: A record containing file name, status, detected headers, flags,
                message, and timestamp for CSV logging.
        - bool: True if compilation succeeded (return code 0); False otherwise.

    Notes
    -----
    - The output executable is named by replacing `.c` with `.out` in `filepath`.
    - Standard output is captured; on failure, standard error is captured instead.
    - A timeout produces a failure with a specific timeout message.
    """
    filename = os.path.basename(filepath)
    headers = detect_headers(filepath)
    output_file = filepath.replace('.c', '.out')  # Final output path for gcc
    cmd = ['gcc', filepath, '-o', output_file]
    extra_flags = []

    # ---------------------------------------------------------
    # Detect required libraries based on included headers/content
    # ---------------------------------------------------------
    # NOTE: This is heuristic and intentionally conservative.
    # It adds only widely expected flags for common headers.

    # Libraries without pkg-config
    # -lm for math functions if math.h detected.
    if any(h.strip() == 'math.h' for h in headers):
        extra_flags.append('-lm')

    # -ljpeg for libjpeg if jpeglib.h detected.
    if any(h in ['jpeglib.h'] for h in headers):
        extra_flags.append('-ljpeg')

    # -lX11 for X11 core headers.
    if any(h in ['X11/Xlib.h', 'X11/Xutil.h'] for h in headers):
        extra_flags.append('-lX11')

    # -lXext for X11 extension headers.
    if any('X11/extensions/' in h for h in headers):
        extra_flags.append('-lXext')

    # pkg-config for proper libraries
    # OpenCV 4.x headers usually live under opencv2/
    if any('opencv2/' in h for h in headers):
        extra_flags += get_pkg_flags('opencv4')

    # SDL2 headers live under SDL2/
    if any('SDL2/' in h for h in headers):
        extra_flags += get_pkg_flags('sdl2')

    # libxml2 commonly exposes headers under libxml/
    if any('libxml/' in h for h in headers):
        extra_flags += get_pkg_flags('libxml-2.0')

    # json-c can be included as <json-c/...>
    if any('json-c' in h for h in headers):
        extra_flags += get_pkg_flags('json-c')

    # libzip main include is zip.h
    if any('zip.h' in h for h in headers):
        extra_flags += get_pkg_flags('libzip')

    # Special case for libcurl: detect via header or common API symbol fragment.
    try:
        content = open(filepath, 'r', errors='ignore').read()
        if any('curl/' in h for h in headers) or 'curl_easy_' in content:
            extra_flags += get_pkg_flags('libcurl')
    except Exception:
        # Silently ignore any read errors; the compile step will reflect issues anyway.
        pass

    # Extend the gcc command with all inferred flags.
    cmd += extra_flags

    # ---------------------------------
    # Execute compilation with a timeout
    # ---------------------------------
    try:
        # capture_output=True captures both stdout/stderr; text=True decodes to str.
        result = subprocess.run(cmd, capture_output=True, text=True, timeout=20)
        success = result.returncode == 0
        # On success, prefer stdout; on failure, show stderr for diagnostic messages.
        output = result.stderr.strip() if not success else result.stdout.strip()
    except subprocess.TimeoutExpired:
        success = False
        output = '⏱ Compilation timed out.'

    # -------------------------------
    # Print human-friendly status line
    # -------------------------------
    symbol = '✅' if success else '❌'
    color = Fore.GREEN if success else Fore.RED
    print(f"{color}{symbol} {filename} - {'Success' if success else 'Fail'}")
    if output:
        # Print compiler messages in yellow to separate from status line color.
        print(Fore.YELLOW + output)

    # -------------------------------------------------
    # Sort artifacts: move success, copy original on fail
    # -------------------------------------------------
    if success:
        os.makedirs(success_dir, exist_ok=True)
        compiled_output = filepath.replace('.c', '.out')
        # Move the produced executable into the success directory.
        shutil.move(compiled_output, str(os.path.join(success_dir, os.path.basename(compiled_output))))
    else:
        os.makedirs(failed_dir, exist_ok=True)
        # Copy the original source to the failed directory for later inspection.
        shutil.copy(filepath, str(os.path.join(failed_dir, filename)))

    # --------------------------------------
    # Build a structured row for CSV logging
    # --------------------------------------
    return {
        'file': filename,
        'status': 'Success' if success else 'Fail',
        'headers': ';'.join(headers),
        'flags': ' '.join(extra_flags),
        'message': output,
        'timestamp': datetime.now().isoformat()
    }, success


def main():
    """Entry point: parse CLI args, compile all `.c` files, write CSV, and print a summary.

    Command-line arguments
    ----------------------
    --input  / -i : str (required)
        Directory containing `.c` files to compile.
    --output / -o : str (default: ./failed)
        Directory to which failed source files are copied.
    --success / -s : str (default: ./success)
        Directory to which successful `.out` executables are moved.

    Behavior
    --------
    - Validates that the input directory exists.
    - Collects all files ending with `.c` (non-recursive).
    - Compiles each file and accumulates results.
    - Writes `compile_results.csv` with a header row and one row per compilation.
    - Prints a colorized summary, plus the destinations for success/failed artifacts.

    Exit Codes
    ----------
    This function does not explicitly set a process exit code; it prints results
    and returns. The script naturally exits with code 0 unless an unhandled
    exception occurs.
    """
    parser = argparse.ArgumentParser(description="Compile all .c files and log results.")
    parser.add_argument('--input', '-i', required=True, help='Directory containing .c files to compile')
    parser.add_argument('--output', '-o', default='./failed', help='Directory to copy failed files')
    parser.add_argument('--success', '-s', default='./success', help='Directory to store successful .out files')
    args = parser.parse_args()

    input_dir = args.input
    failed_dir = args.output
    success_dir = args.success

    # Validate input directory before proceeding.
    if not os.path.isdir(input_dir):
        print(Fore.RED + f"❌ Input directory '{input_dir}' does not exist.")
        return

    # Non-recursive enumeration of `.c` files in the input directory.
    c_files = [os.path.join(input_dir, f) for f in os.listdir(input_dir) if f.endswith('.c')]
    if not c_files:
        print(Fore.YELLOW + "⚠ No .c files found.")
        return

    print(Fore.CYAN + f"📁 Compiling {len(c_files)} .c files in {input_dir}...\n")

    results = []
    success_count = 0
    fail_count = 0

    # Compile each file and collect per-file results.
    for filepath in c_files:
        result, success = compile_single_file(filepath, failed_dir, success_dir)
        results.append(result)
        if success:
            success_count += 1
        else:
            fail_count += 1

    # --------------------------------------------
    # Persist results for later analysis/tracking.
    # --------------------------------------------
    with open(CSV_FILE, 'w', newline='') as csvfile:
        fieldnames = ['file', 'status', 'headers', 'flags', 'message', 'timestamp']
        writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
        writer.writeheader()
        for row in results:
            writer.writerow(row)

    # ----------------
    # Final user output
    # ----------------
    print(Fore.GREEN + f"\n📄 Results saved to {CSV_FILE}")
    print(Fore.MAGENTA + f"📂 Failed files copied to: {failed_dir}/")
    print(Fore.GREEN + f"📦 Successful output files moved to: {success_dir}/")
    print(Fore.BLUE + f"\n🧾 Summary: {len(c_files)} files compiled — "
          f"{Fore.GREEN}{success_count} succeeded{Fore.RESET}, "
          f"{Fore.RED}{fail_count} failed{Fore.RESET}.")


if __name__ == "__main__":
    # Standard Python module guard for CLI execution.
    main()
