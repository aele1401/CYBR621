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

The user has already shared screenshots covering repository contents, permissions/SARIF, compilation failures and temporary-copy experiments, runtime identity/newline behavior, and final Git/mode check. Preserve the original image files from the conversation and attach/embed roughly 5–7 of them in the submitted assignment. Add a screenshot of a significant AI response from this conversation if the instructor expects an AI-response image. Do not present a text transcription as a screenshot.

1. **Repository/Codespace listing** — captured. Shows path, branch, root Lab 2 contents, missing `assistant1` binary, and existing artifacts. Meaning: documents the working Codespace snapshot.
2. **Baseline source/compile evidence** — captured. Includes grep/source and compile failures. Meaning: shows visible source operations and confirms both .c files are not currently buildable; retain errors accurately.
3. **Significant AI response** — still needs a screenshot captured from the AI conversation. Meaning: preserves the claim text evaluated against code and runtime evidence.
4. **CodeQL findings** — captured. SARIF count is 2: `cpp/potentially-dangerous-function` at `assistant2.c:66` and `cpp/world-writable-file-creation` at `assistant2.c:58`. Meaning: records findings from the checked-in SARIF; scan/source currency is uncertain.
5. **umask/stat permissions** — captured, but multiple captures show differing log modes. Use the final/latest mode capture and note the earlier 666 and 660 observations. Meaning: evidence of observed deployment state with unexplained variation, not a stable permission guarantee.
6. **Claim validation runtime evidence** — captured. Existing binary accepted an arbitrary username and newline-forged text. Meaning: confirms those behaviors for the prebuilt executable; binary/source match was not established.
7. **Optional STRIDE experiment** — captured. Oversized input rejection, temporary-log growth, and append to temporary copy. Meaning: individual input bound and current-account ability to append to its test copy; not proof of exhaustion or unauthorized tampering.

The captures were shown in chat, not committed as image files in the GitHub repository. Manually attach/embed the chosen 5–7 actual screenshots in the submission and keep the explanatory “What I tested / What happened / What it means” captions with each.


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

## Screenshot Checklist (Capture 5–7)

The checklist below initially describes desired screenshots. Status is updated in the Live Codespace Results section above; embed the actual originals manually, since screenshots shared in chat are not automatically stored in the repository.

### 1. Codespace and repository contents

- **Capture:** VS Code/Codespace Explorer showing repository/branch, `assistant1.c`, `assistant2.c`, `codeql-results.sarif`, `README.MD`, `userlog.txt`, plus terminal prompt and `pwd`/`ls -l` output.
- **What I tested:** Whether the existing Lab 2 Codespace contains the artifacts required for Lab 3.
- **What happened:** Not captured from this chat. The GitHub `main` listing contains `assistant2`/`assistant2.o`, but not `assistant1`/`assistant1.o`; terminal state must establish whether the Codespace differs.
- **What it means:** It documents the actual baseline environment and the source/binary mismatch, if present.

### 2. Baseline source and grep evidence

- **Capture:** Terminal with both required grep commands above and the relevant source context (`sed -n '1,100p' assistant2.c`; `cat assistant1.c`).
- **What I tested:** Whether source shows bounds, CR/LF handling, file writes, timestamp conversion, or obvious copy functions.
- **What happened:** Repository text has direct `fprintf` input in the assistant1 fragment; `assistant2` has bounded lengths and `snprintf`, but no CR/LF check. The expected matching lines are transcribed above; take actual terminal output.
- **What it means:** It records code behavior and the limitation that `assistant1.c` is incomplete.

### 3. Significant AI response

- **Capture:** The AI answer for Prompt 2 (log injection) from this chat, or paste the exact answer from this report into the AI conversation and capture that answer in the same view as the prompt.
- **What I tested:** Whether a detailed answer's CR/LF claim matches validation and log-write code.
- **What happened:** The recorded response says `assistant2` does not reject message CR/LF and that runtime confirmation is pending.
- **What it means:** It preserves the AI claim being evaluated; source and runtime evidence must remain distinguishable.

### 4. CodeQL findings

- **Capture:** Terminal showing both `jq` commands and the SARIF rule IDs, messages, paths, and lines.
- **What I tested:** What the checked-in CodeQL run actually reported.
- **What happened:** GitHub SARIF parses to two results: `cpp/world-writable-file-creation` at `assistant2.c:58` and `cpp/potentially-dangerous-function` at `assistant2.c:66`.
- **What it means:** The results identify review targets; they do not alone prove live file exposure or an exploitable thread race.

### 5. `umask` and log permissions

- **Capture:** One terminal view with `umask`, `stat -c '%A %a %n' userlog.txt`, and `ls -ld .` (parent directory).
- **What I tested:** Actual mode and directory access in the Codespace.
- **What happened:** Not available from GitHub repository metadata; fill this in verbatim from the terminal. Do not substitute the instructor's example mode.
- **What it means:** These values, along with owner/group and effective identity, determine whether other accounts can read, write, or replace the log in this deployment.

### 6. Evidence validating the CR/LF claim

- **Capture:** Run `assistant2` only if the Codespace binary is present and corresponds to the inspected source. Use a temporary directory and a copy of the log so the tracked Lab 2 log stays unchanged:

```bash
BIN="$PWD/assistant2"
tmpdir=$(mktemp -d)
cp userlog.txt "$tmpdir/userlog.txt"
( cd "$tmpdir" && "$BIN" student1 $'normal event\nFORGED administrator event' && cat userlog.txt )
```

- **What I tested:** Whether an embedded newline is stored as a separate apparent record by the available executable.
- **What happened:** Not executed here. Capture the actual result; first verify that the binary exists and matches the checked-in source/build. If it rejects the input, record that result and investigate source/binary divergence.
- **What it means:** A second apparent line would confirm log forging for that executable and input. It would not prove remote access or acceptance of a forged operating-system identity.

### 7. Optional controlled STRIDE evidence

- **Capture:** In the same temporary directory, show a caller-selected identity and owner-side tampering of the copied log; also show actual `ls -l assistant1 assistant2` separately for SUID/SGID review.

```bash
( cd "$tmpdir" && "$BIN" administrator 'Successful login' && tail -5 userlog.txt )
( cd "$tmpdir" && echo 'TAMPERED LOG ENTRY' >> userlog.txt && tail -5 userlog.txt )
ls -l assistant1 assistant2
```

- **What I tested:** Whether the executable records an arbitrary accepted username and whether the current account can modify a file it owns/can write; whether live binaries display SUID/SGID bits.
- **What happened:** Not executed here. Record the terminal result; the GitHub snapshot alone cannot establish live ownership, permissions, or SUID/SGID state.
- **What it means:** A caller-chosen label proves only that the logger accepts that label. Appending to a copied log proves only that this account can modify that copy. Neither proves remote or unauthorized access. A permission string with `s` would require further privileged-context analysis.

### Optional controlled length/availability check

Use a temporary copied log and do not repeat tests against the live Lab 2 file. Test `assistant2` with 5,000 characters; by source it should reject a message above 255 characters before writing. The unavailable/incomplete `assistant1` source/binary cannot support the proposed long-input runtime experiment in this repository snapshot. A rejected oversized input is not evidence of buffer overflow; repeated accepted records still require storage controls.

## Submission Completion Checks

- Main report includes the title/mission, repository and baseline, all five AI responses, claims and independent verification, a classification table, system model/assets/boundaries, six STRIDE categories, threat/mitigation table, three priorities, five reflections, recommendation, and screenshot list.
- CodeQL count/result contents were parsed from the tracked SARIF: 2 results.
- Required live `ls -l`, `umask`, `stat`, executable/SUID inspection, and runtime experiments remain for the Codespace.
- Capture the five required evidence screenshots plus one validating-claim screenshot; optional seventh screenshot may document safe STRIDE experiments.
- Do not change Lab 2 source. Keep any experiments in a temporary working directory and do not commit the temporary test log.


