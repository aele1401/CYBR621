#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFFER_SIZE 8192

static int safe_close(int fd)
{
    int rc;

    if (fd < 0)
        return 0;

    do {
        rc = close(fd);
    } while (rc == -1 && errno == EINTR);

    return rc;
}

int main(int argc, char *argv[])
{
    int src_fd = -1;
    int dst_fd = -1;
    char buffer[BUFFER_SIZE];

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return EXIT_FAILURE;
    }

    /* Open source file for reading */
    do {
        src_fd = open(argv[1], O_RDONLY);
    } while (src_fd == -1 && errno == EINTR);

    if (src_fd == -1) {
        perror("Failed to open source file");
        return EXIT_FAILURE;
    }

    /*
     * Create destination file:
     * - O_CREAT: create if it does not exist
     * - O_EXCL : fail if it already exists
     * - 0600   : owner read/write only
     */
    do {
        dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, 0600);
    } while (dst_fd == -1 && errno == EINTR);

    if (dst_fd == -1) {
        perror("Failed to create destination file");
        safe_close(src_fd);
        return EXIT_FAILURE;
    }

    for (;;) {
        ssize_t bytes_read;

        /* Retry interrupted reads */
        do {
            bytes_read = read(src_fd, buffer, sizeof(buffer));
        } while (bytes_read == -1 && errno == EINTR);

        if (bytes_read == 0) {
            /* EOF */
            break;
        }

        if (bytes_read == -1) {
            perror("Read error");
            safe_close(src_fd);
            safe_close(dst_fd);
            return EXIT_FAILURE;
        }

        ssize_t total_written = 0;

        while (total_written < bytes_read) {
            ssize_t bytes_written;

            do {
                bytes_written = write(
                    dst_fd,
                    buffer + total_written,
                    (size_t)(bytes_read - total_written)
                );
            } while (bytes_written == -1 && errno == EINTR);

            if (bytes_written == -1) {
                perror("Write error");
                safe_close(src_fd);
                safe_close(dst_fd);
                return EXIT_FAILURE;
            }

            total_written += bytes_written;
        }
    }

    if (safe_close(src_fd) == -1) {
        perror("Error closing source file");
        safe_close(dst_fd);
        return EXIT_FAILURE;
    }

    if (safe_close(dst_fd) == -1) {
        perror("Error closing destination file");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}