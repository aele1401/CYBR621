# Lab 3 Evidence and Screenshot Checklist

## Live Codespace Results Supplied by User (6 October 2026)

The following are transcriptions of screenshots/terminal output the user supplied from `/workspaces/CYBR621/lab2`, branch `main`. They are actual reported observations, not recreated command output. Original Lab 2 source files were not edited. At the final check, `git status --short` produced no output.

### Compilation and buildability

- `gcc -fsyntax-only assistant1.c` exited 1. The compiler reported unknown type `FILE`, implicit declaration of `fopen`, and top-level `if`/return syntax errors. The tracked file is a fragment, not a compilable translation unit.
- Compiling `assistant2.c` failed at line 102: “expected declaration or statement at end of input.” The attempted temporary output binary was not created; subsequent calls to it returned “No such file or directory.”
- Therefore the actual runtime experiments used the preexisting `assistant2` executable. Its timestamp predates `assistant2.c`; repository screenshot timestamps also show the CodeQL database/SARIF predating the source. No evidence establishes that either binary or SARIF corresponds to the exact current source. The following runtime results must be attributed to the prebuilt binary only.

### Prebuilt binary behavior in isolated test directory

The user copied `userlog.txt` to a temporary directory and ran the existing `assistant2` by absolute path from that directory. The captured output shows:

- `administrator "Successful login"` was accepted and wrote a record with user label `administrator`.
- A `student1` message containing newline plus `FORGED administrator event` was accepted. The temporary log showed the second text as a separate apparent line.
- A 5,000-character message was rejected with `Error: Invalid log message. Must be non-empty and under 256 characters.`; reported exit status was 1.
- Copied log size was 146 bytes before these accepted tests and 304 bytes afterwards. This confirms individual input bounds on this binary do not bound aggregate log growth; it does not demonstrate storage exhaustion.
- `TAMPERED LOG ENTRY` was appended to the temporary copy, and appeared in its tail. This demonstrates ability to modify that copy in the test account/context only. It does not demonstrate remote or unauthorized tampering of the original log.

### Permission and privilege captures

- Captured `umask`: `0022`.
- Permission observations are inconsistent across screenshots: one showed `userlog.txt` mode 666; a later same-run check showed 660; the final capture showed `userlog.txt` 600, `assistant2` 777, and `assistant2.c` 666. Cause is unknown. Report the final capture as the latest observation and disclose prior drift; do not infer a stable file mode.
- `assistant1` executable was absent. `assistant2` was executable and mode 777 in the final capture. No SUID/SGID marker was visible in its mode string. The observed broad write permission on the binary is a code-integrity concern in a shared context, but no attacker action, setuid execution, or privilege escalation was demonstrated.
- Final clean `git status --short` is evidence that no tracked modifications remained at that moment.

## Screenshot/Evidence Checklist Status

**Complete:** Seven screenshots are stored in lab3/screenshots/ and embedded with the required captions in [LAB3_FINAL.md](LAB3_FINAL.md). They show repository contents, permissions and CodeQL, both source compilation failures, runtime tests of the prebuilt binary, final Git status, and the AI response. No additional screenshots are required.

## Evidence Provenance and Limits

Repository inspected: [`aele1401/CYBR621`](https://github.com/aele1401/CYBR621), `main`, `lab2/`. Initial repository inspection used GitHub contents/SARIF. The user subsequently supplied terminal screenshots and output from the active Codespace; those live results are transcribed in the addendum below. Any older checklist sentence saying all commands remain pending is superseded by that addendum.

The final analysis is based on checked-in `assistant1.c` (351 bytes), `assistant2.c` (3,357 bytes), `README.MD`, `userlog.txt` (146 bytes), and `codeql-results.sarif` (180,478 bytes). Repository file listing shows `assistant2` and `assistant2.o`; it does not show `assistant1` or `assistant1.o`. It also lists `codeql-db/`, `codeql-db-assistant2/`, and `codeql-db-remediated/`. The exact content snapshots can be reviewed at [assistant1.c](https://github.com/aele1401/CYBR621/blob/main/lab2/assistant1.c), [assistant2.c](https://github.com/aele1401/CYBR621/blob/main/lab2/assistant2.c), [SARIF](https://github.com/aele1401/CYBR621/blob/main/lab2/codeql-results.sarif), and [userlog.txt](https://github.com/aele1401/CYBR621/blob/main/lab2/userlog.txt).

## Baseline Source Search Results

These matches were calculated from the actual checked-in source text, with 1-based line numbers. They are not terminal screenshots.

### `grep -nE 'fopen|fprintf|localtime|strlen|isalnum|fflush' assistant1.c assistant2.c`

```text
assistant1.c:1:FILE *file = fopen("userlog.txt", "a");
assistant1.c:7:if (fprintf(file, "%s: %s\n", argv[1], argv[2]) < 0) {
assistant2.c:15:    if (username == NULL || strlen(username) == 0 || strlen(username) >= MAX_USERNAME_LEN) {
assistant2.c:19:        if (!isalnum((unsigned char)username[i]) && username[i] != '_') {
assistant2.c:30:    if (message == NULL || strlen(message) == 0 || strlen(message) >= MAX_MESSAGE_LEN) {
assistant2.c:39:        fprintf(stderr, "Usage: %s <username> <log_message>\n", argv[0]);
assistant2.c:48:        fprintf(stderr, "Error: Invalid username. Must be alphanumeric/underscores and under %d characters.\n", MAX_USERNAME_LEN);
assistant2.c:53:        fprintf(stderr, "Error: Invalid log message. Must be non-empty and under %d characters.\n", MAX_MESSAGE_LEN);
assistant2.c:58:    FILE *file = fopen("userlog.txt", "a");
assistant2.c:66:    struct tm *t = localtime(&now);
assistant2.c:70:        fprintf(stderr, "Error generating timestamp.\n");
assistant2.c:82:        fprintf(stderr, "Error: Formatting log entry failed or data was truncated.\n");
```

### `grep -nE 'strcpy|strcat|sprintf|gets|memcpy' assistant1.c assistant2.c`

No matches in either source file. `assistant2.c` does contain `snprintf` and checks its result; absence of the exact searched names does not by itself prove safety.

### Source interpretation

- `assistant1.c` is a fragment beginning with `FILE *file = ...`; it lacks includes, a function/`main`, and declarations visible in the checked-in file. It directly passes `argv` values to `%s` in `fprintf`, with no visible fixed-size application buffer. It is not a valid basis for claiming full-program build or runtime behavior.
- `assistant2.c` requires three `argc` values, bounds username and message lengths (maximum accepted lengths 31 and 255), constrains username characters, formats into a bounded local array using `snprintf`, checks its return, then writes with `fputs`. It does not reject CR or LF from the message.
- Both source files use the relative path `userlog.txt`; `assistant2.c` calls `localtime()` once in its one-shot `main` flow.

## Checked-In CodeQL Results

Parsed `runs[0].results` from the actual `codeql-results.sarif`: **2 results**, not the instructor sample's 3.

| Rule ID | Exact SARIF message | File and line | Interpretation |
|---|---|---|---|
| `cpp/world-writable-file-creation` | `A file may be created here with mode 0666, which would make it world-writable.` | `assistant2.c:58` | Potential creation-mode concern. It does not establish the live mode after `umask`, owner/group, directory policy, or ACLs. |
| `cpp/potentially-dangerous-function` | `Call to 'localtime' is potentially dangerous.` | `assistant2.c:66` | Thread-safety/API concern; inspect execution architecture. Not proof of a concurrent race or exploit in this one-shot CLI. |

No buffer-overflow result is present in the two reported SARIF results. A SARIF result list is not a complete security proof; the result set only describes this scan and analyzed code.

## Checked-In Log Snapshot

The GitHub snapshot of `userlog.txt` is 146 bytes and contains two lines:

```text
[2026-10-04 01:36:02] User: student2 - Message: User successfully logged in
[2026-10-04 01:38:14] User: student2 - Message: Sanitized normal test
```

This is repository content, not live output. It gives no owner, mode, ACL, or evidence of who authored the records. It contains no observed forged line.

## Linux API Validation

- [`fopen(3)`](https://man7.org/linux/man-pages/man3/fopen.3.html): mode `a` appends and creates a missing file; created-file permission bits start from `0666` filtered by the process `umask`.
- [`umask(2)`](https://man7.org/linux/man-pages/man2/umask.2.html): the process mask clears requested permission bits during creation.
- [`localtime(3)`](https://man7.org/linux/man-pages/man3/localtime.3.html): `localtime()` returns static storage and is not thread-safe; `localtime_r()` writes to caller-provided storage.
- [`open(2)`](https://man7.org/linux/man-pages/man2/open.2.html): `O_NOFOLLOW` rejects a symbolic link in the final path component. Correct descriptor-based file handling still requires a trusted directory and policy for existing files.

An existing file is not made private merely because a future `fopen(..., "a")` requests append mode. Effective UID/GID, existing owner/mode/ACL, parent-directory permissions and current working directory affect access and replacement. None of these live Codespace attributes is present in the fetched GitHub content.

## Commands to Capture in the Codespace

Run in the Lab 2 Codespace terminal and capture the actual output, including errors or missing files. Do not edit either C source file.

```bash
pwd
ls -l
grep -nE 'fopen|fprintf|localtime|strlen|isalnum|fflush' assistant1.c assistant2.c
grep -nE 'strcpy|strcat|sprintf|gets|memcpy' assistant1.c assistant2.c
jq '.runs[0].results | length' codeql-results.sarif
jq -r '.runs[0].results[] | [.ruleId,.message.text,.locations[0].physicalLocation.artifactLocation.uri,.locations[0].physicalLocation.region.startLine] | @tsv' codeql-results.sarif
umask
stat -c '%A %a %n' userlog.txt
ls -l assistant1 assistant2
```

If `assistant1` is absent, do not treat that as a successful baseline run or build a replacement for this lab. Record its absence and the incomplete source. If current Codespace contents differ from the GitHub `main` snapshot, preserve the branch/commit or file state shown by the terminal before analyzing it.

## Screenshot Checklist Reference

The screenshot set is complete and embedded in LAB3_FINAL.md. The older capture instructions in this evidence log are retained only as a record of the original collection plan; they are superseded by the captured results and captions in the final report.

## Submission Completion Checks

- Main report includes the title/mission, repository and baseline, all five AI responses, claims and independent verification, a classification table, system model/assets/boundaries, six STRIDE categories, threat/mitigation table, three priorities, five reflections, recommendation, and screenshot list.
- CodeQL count/result contents were parsed from the tracked SARIF: 2 results.
- Live umask, stat, SARIF inspection, compilation attempts, temporary-copy experiments, and final Git status have been captured in the supplied screenshots and are documented above. SUID/SGID review is limited to the visible mode string; no privileged execution was demonstrated.
- Seven screenshots have been added under lab3/screenshots/ and embedded in the report.
- The source files were not modified; runtime/tampering tests used a temporary copy of the log.


