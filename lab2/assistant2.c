#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>

#define MAX_USERNAME_LEN 32
#define MAX_MESSAGE_LEN 256

/**
 * Validates that the username is non-empty, within the allowed length,
 * and contains only alphanumeric characters or underscores.
 */
int validate_username(const char *username) {
    if (username == NULL || strlen(username) == 0 || strlen(username) >= MAX_USERNAME_LEN) {
        return 0;
    }
    for (int i = 0; username[i] != '\0'; i++) {
        if (!isalnum((unsigned char)username[i]) && username[i] != '_') {
            return 0;
        }
    }
    return 1;
}

/**
 * Validates that the log message is non-empty and within the allowed length.
 */
int validate_message(const char *message) {
    if (message == NULL || strlen(message) == 0 || strlen(message) >= MAX_MESSAGE_LEN) {
        return 0;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    // Ensure the exact number of expected command-line arguments is provided
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <username> <log_message>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *username = argv[1];
    const char *message = argv[2];

    // Validate inputs defensively to prevent injection and malformed entries
    if (!validate_username(username)) {
        fprintf(stderr, "Error: Invalid username. Must be alphanumeric/underscores and under %d characters.\n", MAX_USERNAME_LEN);
        return EXIT_FAILURE;
    }

    if (!validate_message(message)) {
        fprintf(stderr, "Error: Invalid log message. Must be non-empty and under %d characters.\n", MAX_MESSAGE_LEN);
        return EXIT_FAILURE;
    }

    // Open the log file securely in append mode
    FILE *file = fopen("userlog.txt", "a");
    if (file == NULL) {
        perror("Error opening log file");
        return EXIT_FAILURE;
    }

    // Retrieve the current local time for audit logging
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[64];
    
    if (t == NULL || strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t) == 0) {
        fprintf(stderr, "Error generating timestamp.\n");
        if (fclose(file) != 0) {
            perror("Error closing file after timestamp failure");
        }
        return EXIT_FAILURE;
    }

    // Construct the log entry safely using snprintf to entirely prevent buffer overflows
    char log_entry[MAX_USERNAME_LEN + MAX_MESSAGE_LEN + 128];
    int written = snprintf(log_entry, sizeof(log_entry), "[%s] User: %s - Message: %s\n", time_str, username, message);

    if (written < 0 || (size_t)written >= sizeof(log_entry)) {
        fprintf(stderr, "Error: Formatting log entry failed or data was truncated.\n");
        if (fclose(file) != 0) {
            perror("Error closing file after formatting failure");
        }
        return EXIT_FAILURE;
    }

    // Write the formatted entry to the file and check for I/O errors
    if (fputs(log_entry, file) == EOF) {
        perror("Error writing to log file");
        if (fclose(file) != 0) {
            perror("Error closing file after write failure");
        }
        return EXIT_FAILURE;
    }

    // Safely close the file and check for flushing errors
    if (fclose(file) != 0) {
        perror("Error closing log file");
        return EXIT_FAILURE;
    }