#include <errno.h>
#include <fcntl.h>
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

static void write_error(const char *message)
{
    const char *p = message;

    while (*p != '\0') {
        ssize_t written = write(STDERR_FILENO, p, 1);

        if (written < 0) {
            if (errno == EINTR)
                continue;
            break;
        }

        if (written == 0)
            break;

        p += written;
    }
}

int main(int argc, char *argv[])
{
    int src_fd = -1;
    int dst_fd = -1;
    char buffer[BUFFER_SIZE];

    /* Validate command-line arguments. */
    if (argc != 3) {
        write_error("Usage: ./improved <source> <destination>\n");
        return 1;
    }

    /* Open source file for reading. */
    do {
        src_fd = open(argv[1], O_RDONLY);
    } while (src_fd == -1 && errno == EINTR);

    if (src_fd == -1) {
        write_error("Failed to open source file\n");
        return 1;
    }

    /*
     * Create destination file:
     * O_WRONLY - write-only
     * O_CREAT  - create if it does not exist
     * O_EXCL   - fail if it already exists
     * 0600     - owner read/write only
     */
    do {
        dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, 0600);
    } while (dst_fd == -1 && errno == EINTR);

    if (dst_fd == -1) {
        write_error("Failed to create destination file\n");
        safe_close(src_fd);
        return 1;
    }

    /* Copy the file. */
    for (;;) {
        ssize_t bytes_read;

        /* Retry interrupted reads. */
        do {
            bytes_read = read(src_fd, buffer, sizeof(buffer));
        } while (bytes_read == -1 && errno == EINTR);

        /* End of file. */
        if (bytes_read == 0)
            break;

        if (bytes_read == -1) {
            write_error("Read error\n");
            safe_close(src_fd);
            safe_close(dst_fd);
            return 1;
        }

        /*
         * Handle partial writes by continuing until
         * all bytes read from the source have been written.
         */
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
                write_error("Write error\n");
                safe_close(src_fd);
                safe_close(dst_fd);
                return 1;
            }

            /*
             * A zero-byte write while data remains would
             * leave total_written unchanged and could cause
             * an infinite loop.
             */
            if (bytes_written == 0) {
                write_error("Write returned zero bytes\n");
                safe_close(src_fd);
                safe_close(dst_fd);
                return 1;
            }

            total_written += bytes_written;
        }
    }

    /* Close source file. */
    if (safe_close(src_fd) == -1) {
        write_error("Error closing source file\n");
        safe_close(dst_fd);
        return 1;
    }

    /* Close destination file. */
    if (safe_close(dst_fd) == -1) {
        write_error("Error closing destination file\n");
        return 1;
    }

    return 0;
}