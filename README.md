# Codegen & Compile Utilities

Small tools to (1) batch-generate code from task prompts using OpenAI, (2) compile many C sources with inferred flags, and (3) sanity-check model access.

## Contents
- **`codegen_from_tasks.py`** – reads a CSV `"Task"` column and generates code files via OpenAI Chat Completions.
- **`compile_c_outputdata.py`** – compiles all `.c` files in a directory, infers common linker flags, and logs results.
- **`check_openai_model_access_snippet.py`** – best-effort check that your API key can access a requested model.

## Requirements
- Python 3.10+
- Packages: `openai` (v2+), `colorama`
- Tools (optional but recommended):
  - `gcc` and, if used, `pkg-config` with libraries installed (e.g., `opencv4`, `sdl2`, `libxml-2.0`, `json-c`, `libzip`, `libcurl`)

Install (see requirements.txt):
```bash
pip install openai colorama
pip install openai>=2.0
```

---

## 1) codegen_from_tasks.py

### Purpose
Batch-generate code files from natural-language tasks in a CSV.

### Usage
- Input CSV *must* contain a header column named `Task`.
- API key resolution order: `--api-key` flag → `OPENAI_API_KEY` env var → `.openai-api-key` file.

```bash
# Basic: generate Python code for each task row using gpt-4o
python codegen_from_tasks.py tasks.csv --model gpt-4o --language Python

# Set output directory and print prompts + full responses
python codegen_from_tasks.py tasks.csv --output-dir ./OutputData --verbose

# Request code-only responses with a soft 200-line budget
python codegen_from_tasks.py tasks.csv --code-only --max-lines 200
```

### Notes
- Filenames are inferred from the model response (`# File: ...`) or derived from the task text.
- Per-task errors are logged; the run continues.

---

## 2) compile_c_outputdata.py

### Purpose
Compile all `.c` files in a directory, guess needed flags from `#include`s, log to CSV, and separate success/fail outputs.

### Usage
```bash
python compile_c_outputdata.py --input ./CSource --output ./failed --success ./success
```

### Behavior
- Infers flags: e.g., `-lm`, `-lX11`, `-lXext`, `-ljpeg`, plus `pkg-config` flags for libraries like `opencv4`, `sdl2`, `libxml-2.0`, `json-c`, `libzip`, `libcurl`.
- Timeout: 20s per file.
- Results: `compile_results.csv`, with artifacts moved to `--success` or copied to `--output`.

---

## 3) check_openai_model_access_snippet.py

### Purpose
Best-effort check whether your API key can access a given model.

### Usage
```bash
# Uses OPENAI_API_KEY from environment
python check_openai_model_access_snippet.py --model gpt-4o

# Or pass the key explicitly
python check_openai_model_access_snippet.py --model gpt-4o --api-key "$OPENAI_API_KEY"
```

### Exit Codes
- `0` = available (or check failed-open but didn’t block)
- `1` = definitely not available

---

## Troubleshooting

- **Compilation timeouts**:
  - Reduce input set, or extend the timeout in `compile_c_outputdata.py` (search for `timeout=20`).

---

## Conventions
- Scripts are CLI-friendly and can also be imported as modules.
