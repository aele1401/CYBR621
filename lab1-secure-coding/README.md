# CYBR 621 Lab 1: Benchmarking AI Coding Assistants for Secure Systems Programming

## Overview

This lab evaluated two AI coding assistants, GitHub Copilot and OpenAI Codex, using the same secure Linux C programming task. The goal was to determine how well each assistant handled correctness, security, error handling, code quality, efficiency, and compliance with the requirements.

An earlier ChatGPT solution was also preserved as `ChatGPT.c` from an initial attempt. However, ChatGPT is not considered a coding agent, so GitHub Copilot and OpenAI Codex were used for the formal comparison.

---

## Environment

The lab was completed in GitHub Codespaces using Linux, GCC, and Git.

- **Operating System:** Linux / GitHub Codespaces
- **Programming Language:** C
- **Compiler:** GCC
- **Compiler Flags:** `-Wall -Wextra -pedantic`
- **Version Control:** Git / GitHub
- **System Interface:** POSIX system calls

The development environment was configured and verified before beginning the implementation.

![Environment Setup](https://github.com/aele1401/CYBR621/blob/main/lab1-secure-coding/Images/01-environment-setup.png)

---

## AI Coding Assistants

### GitHub Copilot

GitHub Copilot was used as one of the two formal AI coding assistants. It received the exact same prompt provided to OpenAI Codex.

The generated implementation is available here:

[**View Copilot.c**](./Copilot.c)

### OpenAI Codex

OpenAI Codex was used as the second formal AI coding assistant. It received the exact same prompt provided to GitHub Copilot.

The generated implementation is available here:

[**View Codex.c**](./Codex.c)

### Additional ChatGPT Artifact

An earlier ChatGPT-generated implementation was preserved as:

[**View ChatGPT.c**](./ChatGPT.c)

This file is included to preserve the initial attempt and document the development process. It was not used as one of the two formal coding assistants for the final comparison.

---

## Task and Prompt

Both formal AI coding assistants received the exact same prompt.

The program was required to:

- Use only POSIX system calls (`open`, `read`, `write`, `close`)
- Validate command-line arguments
- Do not overwrite an existing destination file
- Create the destination file with permissions `0600`
- Handle all errors gracefully
- Correctly handle partial reads and writes
- Retry interrupted system calls
- Avoid unsafe C library functions
- Close all file descriptors before exiting
- Compile with GCC on Linux

The complete prompt is preserved in:

[**View prompt.md**](./prompt.md)

The original prompt used for the AI assistants is also documented in the lab evidence below.

![AI Prompt](https://github.com/aele1401/CYBR621/blob/main/lab1-secure-coding/Images/01-environment-setup.png)

---

# Evaluation Method

The implementations were evaluated through compilation, functional testing, edge-case testing, and manual code review.

Each implementation was compiled using:

```bash
gcc -Wall -Wextra -pedantic <source>.c -o <program>