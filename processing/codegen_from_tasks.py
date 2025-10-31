"""codegen_from_tasks.py

Purpose
-------
Batch-generate code files from a CSV list of natural-language tasks by calling
the OpenAI Chat Completions API. For each task, a language-specific prompt is
constructed and sent to the model; the returned content is saved to disk with
a suggested filename inferred from the response or derived from the task.

High-Level Flow
---------------
1) Parse CLI options (input CSV, model, language, output dir, etc.).
2) Read tasks from a CSV column named "Task".
3) Acquire an API key (CLI arg > env var > file).
4) Validate access to the requested model (best-effort).
5) For each task:
   - Build a well-formed prompt.
   - Call the API with retry/backoff on rate limiting / transient errors.
   - Extract a filename from the response (fallback to a safe, derived name).
   - Save the response text to the output directory.
   - Optionally print verbose details.

Notes
-----
- This script assumes network access and valid OpenAI API credentials.
- The CSV must contain a header named "Task".
- Errors per task are logged and do not stop the whole run.

Examples
--------
    # Basic: generate Python code for tasks.csv using gpt-4o
    python codegen_from_tasks.py tasks.csv --model gpt-4o --language Python

    # Specify API key inline and save outputs to a custom directory
    python codegen_from_tasks.py tasks.csv --api-key "$OPENAI_API_KEY" --output-dir ./gen

    # Request concise code only, with a soft 200-line budget
    python codegen_from_tasks.py tasks.csv --code-only --max-lines 200
"""

import argparse
import logging
import csv
import re
import os
import time
import random
# import openai
from openai import OpenAI, RateLimitError, APIError
from openai.types.chat import ChatCompletionUserMessageParam  # , ChatCompletionMessageParam


# ---------- Utility Functions ----------

def extract_suggested_filename(response, default_name, language):
    """Infer a good output filename from the model response text.

    Strategy
    --------
    1) Look for a leading comment line that explicitly suggests a file, in the
       format: `# File: name.ext` or `// File: name.ext`. If present, use it.
    2) Otherwise, derive a safe snake-like filename from the task text and
       append a language-appropriate extension.

    Parameters
    ----------
    response : str
        The raw text returned by the model (may include prose and/or code).
    default_name : str
        A task-derived fallback name used when no explicit filename is found.
    language : str
        Target programming language (used to pick a file extension).

    Returns
    -------
    str
        A filename such as "generate_report.py" or "task_01.c". Path
        separators and non-word characters are stripped to avoid surprises.

    Examples
    --------
    If the response begins with:
        "# File: summarize_text.py"
    this function will return "summarize_text.py".
    """
    pattern = r"(?:#|//)\s*File:\s*(.+)"
    match = re.search(pattern, response)
    if match:
        return match.group(1).strip()

    # Map a subset of languages to conventional file extensions; default to .txt.
    ext = {
        "python": "py", "c": "c", "cpp": "cpp",
        "javascript": "js", "java": "java"
    }.get(language.lower(), "txt")

    # Produce a conservative, filesystem-friendly basename limited to 30 chars.
    safe_name = re.sub(r'\W+', '_', default_name.lower())[:30]
    return f"{safe_name}.{ext}"


def load_tasks_from_csv(file_path):
    """Load a series of natural-language tasks from a CSV file.

    The CSV must include a column header named "Task". Each non-empty row in
    this column becomes one unit of work for code generation.

    Parameters
    ----------
    file_path : str
        Path to the CSV file containing tasks.

    Returns
    -------
    list[str]
        A list of task descriptions (strings) in row order.

    Raises
    ------
    ValueError
        If the CSV lacks a "Task" column.
    """
    tasks = []
    with open(file_path, newline='', encoding='utf-8') as csvfile:
        reader = csv.DictReader(csvfile)
        if "Task" not in reader.fieldnames:
            raise ValueError("CSV must contain a column named 'Task'")
        for row in reader:
            task = row["Task"].strip()
            if task:
                tasks.append(task)
    return tasks


def get_api_key(cli_key=None, default_path=".openai-api-key"):
    """Resolve the OpenAI API key from CLI, environment, or file.

    Precedence
    ----------
    1) Explicit --api-key argument.
    2) Environment variable OPENAI_API_KEY.
    3) Local file at `default_path`.

    Parameters
    ----------
    cli_key : str or None
        API key passed via command-line.
    default_path : str
        Fallback file path used if CLI/env are not provided.

    Returns
    -------
    str
        The resolved API key.

    Raises
    ------
    FileNotFoundError
        If no key is provided in CLI or env and the file doesn't exist.
    """
    if cli_key:
        return cli_key
    if os.getenv("OPENAI_API_KEY"):
        return os.getenv("OPENAI_API_KEY")
    return load_api_key_from_file(default_path)


def load_api_key_from_file(path=".openai-api-key"):
    """Load the OpenAI API key from a plain-text file.

    The file should contain only the key string with no extra decoration.

    Parameters
    ----------
    path : str
        Filesystem path to the key file.

    Returns
    -------
    str
        The API key read from disk.

    Raises
    ------
    FileNotFoundError
        If the key file cannot be located at `path`.
    """
    try:
        with open(path, "r") as f:
            return f.read().strip()
    except FileNotFoundError:
        raise FileNotFoundError(f"API key file not found at {path}")


def check_model_access(client, requested_model):
    """Best-effort check that the requested model is available to this key.

    This is a non-fatal preflight step: if listing models fails, the function
    logs a warning and returns True to avoid blocking execution.

    Parameters
    ----------
    client : openai.OpenAI
        Initialized client used to query `client.models.list()`.
    requested_model : str
        Model ID to validate (e.g., "gpt-4o").

    Returns
    -------
    bool
        True if the model appears in the account's model list or if the check
        cannot be completed; False if it is definitely not available.
    """
    try:
        available_models = client.models.list()
        available_ids = [model.id for model in available_models.data]
        if requested_model not in available_ids:
            print(f"⚠️ Warning: The model '{requested_model}' is not available to your API key.")
            print("📌 Available models you can access:")
            for model_id in sorted(available_ids):
                print(f" - {model_id}")
            print("\n💡 Try using one of the above with --model")
            return False
        return True
    except Exception as e:
        # Fail-open to avoid blocking usage on transient errors or permission issues.
        print(f"Failed to validate model access: {e}")
        return True  # Fail-safe


def generate_prompt(task, language, code_only, max_lines):
    """Construct a crisp, language-specific code-generation prompt.

    The prompt requests: complete, self-contained code; idiomatic style; line
    budget; comments; standard library usage; and a top-of-file filename hint.

    Parameters
    ----------
    task : str
        Natural-language description of the desired program behavior.
    language : str
        Target programming language (e.g., "Python", "C", "JavaScript").
    code_only : bool
        If True, instructs the model to return only a code block (no prose).
    max_lines : int
        Soft line budget for the generated code.

    Returns
    -------
    str
        The fully assembled prompt string ready for API submission.
    """
    prompt = f"""Write a complete program in {language} that will {task}.

The code should be:
- Self-contained and functional
- Written in clear, idiomatic {language}
- Under {max_lines} lines if possible
- Include comments explaining the key parts
- Use only standard {language} libraries
- Suggest a filename at the top in a comment, like: // File: something_representative.c or # File: something_representative.py
"""
    if code_only:
        prompt += "\nOnly return the code block, no explanation."
    return prompt


def call_openai_api(prompt, model, api_key, max_retries=5):
    """Call the Chat Completions API with retries and exponential backoff.

    Retry Policy
    ------------
    - Retries on `openai.RateLimitError` and `openai.APIError`.
    - Exponential backoff (1s, 2s, 4s, ...) plus small random jitter.
    - On unexpected exceptions, the error is raised immediately.

    Parameters
    ----------
    prompt : str
        Prompt content to send in a single-turn chat (user role).
    model : str
        Model ID to query (e.g., "gpt-4o").
    api_key : str
        OpenAI API key used to initialize the client.
    max_retries : int
        Maximum number of attempts before giving up.

    Returns
    -------
    str
        The `.content` field of the first message choice (string).

    Raises
    ------
    Exception
        If all retry attempts are exhausted, an exception is raised.
    """
    client = OpenAI(api_key=api_key)
    retry_delay = 1  # Start with 1 second
    for attempt in range(max_retries):
        try:
            chat_completion_1: ChatCompletionUserMessageParam = {"role": "user", "content": prompt}
            messages: list[ChatCompletionUserMessageParam] = [chat_completion_1]
            response = client.chat.completions.create(
                model=model,
                # messages=[{"role": "user", "content": prompt}],
                messages=messages,
                temperature=0.5,
            )
            return response.choices[0].message.content
        except RateLimitError as e:
            # Transient: exceed quota or concurrency; try again after a delay.
            print(f"⚠️ Rate limit hit. Retrying in {retry_delay:.2f} seconds... (Attempt {attempt + 1}\n{e})")
        except APIError as e:
            # Transient server-side errors (5xx) or gateway errors.
            print(f"⚠️ API error: {e}. Retrying in {retry_delay:.2f} seconds... (Attempt {attempt + 1})")
        except Exception as e:
            # Non-retryable, unexpected error—surface immediately.
            print(f"❌ Unexpected error: {e}")
            raise

        time.sleep(retry_delay)
        retry_delay *= 2  # Exponential backoff
        retry_delay += random.uniform(0, 1)  # Add jitter to avoid thundering herd

    # If here, all attempts failed.
    raise Exception(f"❌ Failed to complete request after {max_retries} attempts.")


# ---------- Main Logic ----------

def main():
    """CLI entry point.

    Responsibilities
    ----------------
    - Parse command-line flags (model, language, output, verbosity, etc.).
    - Configure logging (to file or console).
    - Ensure the output directory exists.
    - Load tasks from the input CSV.
    - Resolve API key and verify model access (best-effort).
    - For each task:
        * Build a prompt and (optionally) print it.
        * Call the API with retry/backoff.
        * Choose a filename and write the response to disk.
        * Emit progress to stdout (and verbose response if requested).

    Exit Behavior
    -------------
    - Returns normally; does not call `sys.exit`.
    - Per-task failures are logged and printed but do not abort the run.
    """
    parser = argparse.ArgumentParser(description="Generate code using ChatGPT from a task list file.")
    parser.add_argument("input_file", help="Path to the file containing tasks (CSV with 'Task' column).")
    parser.add_argument("--model", default="gpt-4o", help="OpenAI model to use (default: gpt-4o)")
    parser.add_argument("--language", default="Python", help="Programming language for the generated code")
    parser.add_argument("--max-lines", type=int, default=500, help="Maximum lines of generated code")
    parser.add_argument("--code-only", action="store_true", help="Only return the code block, no explanation")
    parser.add_argument("--verbose", action="store_true", help="Print detailed output to console")
    parser.add_argument("--log", help="Path to log file (optional)")
    parser.add_argument("--output-dir", default="./OutputData",
                        help="Directory to save generated code files (default: ./OutputData)")
    parser.add_argument("--api-key", help="OpenAI API key (overrides .openai-api-key)")
    args = parser.parse_args()

    # Setup logging: file if provided; otherwise, root logger prints to console.
    if args.log:
        logging.basicConfig(filename=args.log, level=logging.INFO)
    else:
        logging.basicConfig(level=logging.INFO)

    # Ensure output directory exists (no error if already present).
    os.makedirs(args.output_dir, exist_ok=True)

    # Load tasks (will raise if CSV is missing the required "Task" column).
    tasks = load_tasks_from_csv(args.input_file)

    # get api kiy
    # NOTE: Preserving the original variable name and comment per user request.
    api_key = get_api_key(args.api_key)

    # Check model access before generating anything (best-effort, non-fatal).
    client = OpenAI(api_key=api_key)
    if not check_model_access(client, args.model):
        # print(f"Model {args.model} does not exist.")
        return  # or exit(1)

    for i, task in enumerate(tasks, start=1):
        prompt = generate_prompt(task, args.language, args.code_only, args.max_lines)
        if args.verbose:
            print(f"\nTask {i}: {task}")
            print("Prompt sent to model:\n", prompt)
        try:
            response = call_openai_api(prompt, args.model, api_key)
            filename = extract_suggested_filename(response, task, args.language)
            filepath = os.path.join(args.output_dir, filename)

            # Persist the model's raw response as-is (may include code fences or prose).
            with open(filepath, 'w', encoding='utf-8') as f:
                f.write(response)

            print(f"\n✅ Task {i} saved to: {filepath}")
            if args.verbose:
                print("Response from model:\n", response)
            else:
                # Preserve original behavior: also echo response (non-verbose path).
                print(f"\n--- Task {i}: {task} ---\n{response}\n")
        except Exception as e:
            # Log error with task context and notify the user.
            logging.error(f"Error on task {i} ({task}): {e}")
            print(f"❌ Failed to process task {i}. See logs for details.")


if __name__ == "__main__":
    main()
