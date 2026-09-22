# CYB 621 Lab 2
## AI Code Review and Memory-Safety Validation

This repository contains the work for CYB 621 Lab 2.

### Lab Objective

Evaluate two independently generated AI implementations of a Linux C
logging program using:

- GCC compiler warnings
- Boundary and behavioral testing
- AddressSanitizer (ASan)
- UndefinedBehaviorSanitizer (UBSan)
- Manual secure-code review
- Semgrep
- CodeQL
- Linux file-permission analysis
- Remediation and revalidation

### Baseline Implementations

- `assistant1.c` — generated using the baseline AI prompt
- `assistant2.c` — generated using the security-focused AI prompt

The original AI-generated implementations will be preserved before
remediation so that the validation results can be compared against the
post-remediation versions.

### Evidence

Evidence generated during the lab will include:

- Compiler output
- Runtime/boundary testing
- Sanitizer results
- Semgrep results
- CodeQL SARIF results
- File-permission observations
- Post-remediation validation

### Status

Lab setup / baseline preparation.