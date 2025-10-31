"""check_openai_model_access_snippet.py

Purpose
-------
Provide a small, reusable utility for verifying that the current OpenAI API key
can access a requested model ID. The module exposes a single function
`check_model_access(client, requested_model)` and also includes a simple CLI so
you can run it directly from the shell.

Usage
-----
Library usage:
    from openai import OpenAI
    from check_openai_model_access_snippet import check_model_access

    client = OpenAI()  # reads OPENAI_API_KEY from environment
    ok = check_model_access(client, "gpt-4o")
    if not ok:
        print("Please choose a model from the printed list.")

CLI usage:
    # Uses OPENAI_API_KEY from your environment
    python check_openai_model_access_snippet.py --model gpt-4o

    # Or pass an explicit key (overrides environment)
    python check_openai_model_access_snippet.py --model gpt-4o --api-key "$OPENAI_API_KEY"

Notes
-----
- On any unexpected exception (e.g., permissions or network), this function
  **fails open** and returns True after printing a warning, so it does not block
  your program’s execution. This preserves the original behavior.
"""

from __future__ import annotations

import argparse
import os
from typing import Any

# The v2 OpenAI SDK
# Docs: https://github.com/openai/openai-python
from openai import OpenAI


def check_model_access(client: OpenAI, requested_model: str) -> bool:
    """Best-effort check that `requested_model` is accessible with the current key.

    Behavior (unchanged from the original snippet):
    - Lists available models via `client.models.list()`.
    - Prints a warning and the available model IDs if `requested_model` is not found.
    - Returns:
        * False if the requested model is definitely not available.
        * True if it is available.
        * True on *any* exception (fail-safe), after printing the error.

    Parameters
    ----------
    client : openai.OpenAI
        An initialized OpenAI client (already configured with an API key).
    requested_model : str
        The model ID to check, e.g. "gpt-4o" or "o4-mini".

    Returns
    -------
    bool
        True if available (or if the check cannot be completed), False otherwise.
    """
    try:
        # Query the available models for this API key
        available_models = client.models.list()
        available_ids = [model.id for model in available_models.data]

        # If the requested model isn't present, print a friendly listing and return False
        if requested_model not in available_ids:
            print(f"⚠️ Warning: The model '{requested_model}' is not available to your API key.")
            print("📌 Available models you can access:")
            for model_id in sorted(available_ids):
                print(f" - {model_id}")
            print("\n💡 Try using one of the above with --model")
            return False

        # Otherwise, it appears accessible
        return True

    except Exception as e:
        # Preserve original fail-open behavior: do not block the caller on errors
        print(f"Failed to validate model access: {e}")
        return True  # Fail-safe: continue even if check fails


def _build_arg_parser() -> argparse.ArgumentParser:
    """Create the CLI argument parser for standalone usage."""
    parser = argparse.ArgumentParser(
        description="Best-effort check that an OpenAI model is available to your API key."
    )
    parser.add_argument(
        "--model",
        required=True,
        help="Model ID to check (e.g., gpt-4o, gpt-4o-mini, o4-mini).",
    )
    parser.add_argument(
        "--api-key",
        default=None,
        help="Optional OpenAI API key. If omitted, uses OPENAI_API_KEY from the environment.",
    )
    return parser


def _resolve_api_key(cli_key: str | None) -> str | None:
    """Resolve the API key to use (CLI overrides env)."""
    if cli_key:
        return cli_key
    return os.getenv("OPENAI_API_KEY")


def _main() -> int:
    """CLI entry point: build a client, check access, print result, and exit code.

    Exit codes
    ----------
    0 : The model is available (or the check failed open and returned True).
    1 : The model is definitely not available to this key.
    """
    parser = _build_arg_parser()
    args = parser.parse_args()

    api_key = _resolve_api_key(args.api_key)
    if not api_key:
        print("❌ No API key provided. Set OPENAI_API_KEY or pass --api-key.")
        return 1

    # Initialize the OpenAI client
    client = OpenAI(api_key=api_key)

    # Perform the check (function preserves original behavior)
    ok = check_model_access(client, args.model)

    # User-friendly output and exit code
    if ok:
        print(f"✅ Access check: '{args.model}' appears available (or check could not be completed).")
        return 0
    else:
        print(f"❌ Access check: '{args.model}' is not available to this API key.")
        return 1


if __name__ == "__main__":
    raise SystemExit(_main())
