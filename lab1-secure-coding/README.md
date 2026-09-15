# CYBR 621 Lab 1: Benchmarking AI Coding Assistants

## Overview

This lab evaluates two AI coding assistants on the same secure systems programming task. The objective is to compare the generated C implementations for correctness, security, code quality, efficiency, and compliance with the requirements.

The task is to create a secure Linux C program that copies one file to another using POSIX system calls.

## Environment

- Platform: GitHub Codespaces
- Operating System: Linux
- Compiler: GCC
- Language: C
- Compiler flags: `-Wall -Wextra -pedantic`

## AI Assistants

Two AI coding assistants were given the exact same prompt:

1. ChatGPT — saved as `assistant1.c`
2. GitHub Copilot — saved as `assistant2.c`

The original AI-generated implementations were preserved without modification for comparison.

## Task

The complete prompt provided to both assistants is stored in `prompt.md`.

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

Both implementations were compiled and tested using GCC.

## Findings

### Assistant 1 — ChatGPT

The ChatGPT implementation correctly used `O_CREAT | O_EXCL` with mode `0600`, handled interrupted reads and writes, supported partial writes, and avoided unsafe C library functions.

Its main weaknesses were limited error reporting and minimal user feedback. The implementation was simple and straightforward but less polished from a usability perspective.

### Assistant 2 — GitHub Copilot

The Copilot implementation provided more detailed error messages and included retry handling for interrupted `open`, `read`, `write`, and `close` operations. It also used an 8192-byte buffer.

During manual review, a weakness was identified in the partial-write loop: a zero-byte return from `write()` was not explicitly handled. If this occurred while data remained to be written, `total_written` would not increase and the loop could potentially continue indefinitely.

The implementation also used `fprintf()`, `perror()`, and standard-library exit constants despite the prompt specifically requesting the use of only the listed POSIX system calls.

## Improved Implementation

The weaker implementation was used as the starting point for `improved.c`.

The improved version:

- Detects zero-byte writes to prevent a potential infinite loop
- Retains partial-write handling
- Retries interrupted system calls
- Prevents destination-file overwrites using `O_EXCL`
- Creates the destination with `0600` permissions
- Uses `write()` for error output instead of standard I/O functions
- Closes file descriptors before exiting
- Compiles cleanly with GCC using strict warning flags

## Testing

The improved implementation was compiled with:

```bash
gcc -Wall -Wextra -pedantic improved.c -o improved