# CYBR 621 Lab 1: Benchmarking AI Coding Assistants

## Overview

This lab evaluates three AI models on the same secure systems programming task. The objective is to compare the generated C implementations for correctness, security, code quality, efficiency, and compliance with the requirements.

The task is to create a secure Linux C program that copies one file to another using POSIX system calls.

## Environment

- Platform: GitHub Codespaces
- Operating System: Linux
- Compiler: GCC
- Language: C
- Compiler flags: `-Wall -Wextra -pedantic`

## AI Models

The same prompt was used with three AI models:

1. ChatGPT - saved as `ChatGPT.c`
2. GitHub Copilot - saved as `Copilot.c`
3. OpenAI Codex - saved as `Codex.c`

The original AI-generated implementations were preserved without modification for comparison.

The formal coding-agent comparison focuses on GitHub Copilot and OpenAI Codex, following clarification that ChatGPT is not considered a coding agent. The ChatGPT implementation is preserved as an additional third model output and provides additional context for evaluating differences in AI-generated code.

## Task

The complete prompt provided to all three AI models is stored in `prompt.md`.

The program was required to:

- Use only POSIX system calls (`open`, `read`, `write`, `close`)
- Validate command-line arguments
- Prevent overwriting an existing destination file
- Create the destination file with permissions `0600`
- Handle errors gracefully
- Correctly handle partial reads and writes
- Retry interrupted system calls
- Avoid unsafe C library functions
- Close all file descriptors before exiting
- Compile with GCC on Linux

## Evaluation

Each implementation was manually reviewed for:

- Functional correctness
- Security
- Error handling
- File descriptor management
- Partial read/write handling
- Interrupted system call handling
- Use of unsafe or inappropriate functions
- Code quality and readability
- API correctness
- Efficiency
- Compliance with the original prompt

The implementations were compiled and tested using GCC. Testing included normal file-copy operations, destination-file overwrite prevention, invalid input, empty files, and additional edge cases.

## Findings

### ChatGPT

The ChatGPT implementation correctly used `O_CREAT | O_EXCL` with mode `0600`, handled interrupted reads and writes, supported partial writes, and avoided unsafe C library functions.

Its main weaknesses were limited error reporting and minimal user feedback. The implementation was simple and straightforward but less polished from a usability perspective.

The implementation is preserved as `ChatGPT.c` as an additional model output and was not used as one of the two formal coding-agent submissions.

### OpenAI Codex

The Codex implementation correctly used `O_CREAT | O_EXCL` with mode `0600`, handled partial writes, retried interrupted `read()` and `write()` operations, and avoided standard I/O functions by using `write()` for error output.

The implementation also explicitly handled a zero-byte `write()` result. This prevents the partial-write loop from continuing indefinitely when no progress is made.

A prompt-compliance weakness was identified: the implementation did not retry `open()` when it returned `EINTR`, even though the prompt required interrupted system calls to be retried.

Additional edge-case testing showed that when the source path referred to a directory, the destination file was created before the read failure occurred. The resulting empty destination file remained after the failure. This represents an error-handling and cleanup weakness rather than a direct code-execution vulnerability.

### GitHub Copilot

The Copilot implementation correctly prevented destination-file overwrites using `O_CREAT | O_EXCL`, created the destination with `0600` permissions, handled partial writes, and provided useful error messages.

During manual review, a weakness was identified in the partial-write loop: a zero-byte return from `write()` was not explicitly handled. If this occurred while data remained to be written, `total_written` would not increase and the loop could potentially continue indefinitely.

The implementation also used `fprintf()`, `perror()`, and standard-library exit constants despite the prompt specifically requesting the use of only the listed POSIX system calls. These are legitimate C functions, but their use does not fully comply with the specific prompt requirements.

## Improved Implementation

The Copilot implementation was used as the starting point for `improved.c`.

The improved version:

- Detects zero-byte writes to prevent a potential infinite loop
- Retains partial-write handling
- Retries interrupted `open()`, `read()`, and `write()` operations
- Prevents destination-file overwrites using `O_EXCL`
- Creates the destination with `0600` permissions
- Uses `write()` for error output instead of standard I/O functions
- Closes file descriptors before exiting
- Compiles cleanly with GCC using strict warning flags

## Testing

The improved implementation was compiled with:

```bash
gcc -Wall -Wextra -pedantic improved.c -o improved
