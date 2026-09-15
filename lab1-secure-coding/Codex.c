/* copy.c: securely copy SOURCE to DESTINATION */
#include <errno.h>
#include <fcntl.h>
#include <stddef.h>
#include <unistd.h>

#define BUFFER_SIZE 8192

static void write_stderr(const char *message, size_t length)
{
    while (length > 0) {
        ssize_t n = write(STDERR_FILENO, message, length);

        if (n > 0) {
            message += n;
            length -= (size_t)n;
        } else if (n < 0 && errno == EINTR) {
            continue;
        } else {
            return; /* Reporting an error failed; nothing more to do. */
        }
    }
}

static int close_retry(int fd)
{
    int result;

    do {
        result = close(fd);
    } while (result == -1 && errno == EINTR);

    return result;
}

int main(int argc, char *argv[])
{
    char buffer[BUFFER_SIZE];
    int source_fd = -1;
    int dest_fd = -1;
    int status = 1;

    if (argc != 3) {
        write_stderr("Usage: copy SOURCE DESTINATION\n",
                     sizeof("Usage: copy SOURCE DESTINATION\n") - 1);
        return 1;
    }

    source_fd = open(argv[1], O_RDONLY);
    if (source_fd == -1) {
        write_stderr("Error: cannot open source file\n",
                     sizeof("Error: cannot open source file\n") - 1);
        goto cleanup;
    }

    /*
     * O_EXCL together with O_CREAT prevents replacing an existing file.
     * The requested permissions are 0600, subject to the process umask.
     */
    dest_fd = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, 0600);
    if (dest_fd == -1) {
        write_stderr("Error: cannot create destination file\n",
                     sizeof("Error: cannot create destination file\n") - 1);
        goto cleanup;
    }

    for (;;) {
        ssize_t bytes_read;

        do {
            bytes_read = read(source_fd, buffer, sizeof(buffer));
        } while (bytes_read == -1 && errno == EINTR);

        if (bytes_read == 0) {
            status = 0; /* End of file */
            break;
        }

        if (bytes_read == -1) {
            write_stderr("Error: cannot read source file\n",
                         sizeof("Error: cannot read source file\n") - 1);
            goto cleanup;
        }

        {
            size_t offset = 0;
            size_t remaining = (size_t)bytes_read;

            while (remaining > 0) {
                ssize_t bytes_written;

                do {
                    bytes_written = write(dest_fd, buffer + offset, remaining);
                } while (bytes_written == -1 && errno == EINTR);

                if (bytes_written <= 0) {
                    write_stderr("Error: cannot write destination file\n",
                                 sizeof("Error: cannot write destination file\n") - 1);
                    goto cleanup;
                }

                offset += (size_t)bytes_written;
                remaining -= (size_t)bytes_written;
            }
        }
    }

cleanup:
    if (dest_fd != -1 && close_retry(dest_fd) == -1) {
        write_stderr("Error: cannot close destination file\n",
                     sizeof("Error: cannot close destination file\n") - 1);
        status = 1;
    }

    if (source_fd != -1 && close_retry(source_fd) == -1) {
        write_stderr("Error: cannot close source file\n",
                     sizeof("Error: cannot close source file\n") - 1);
        status = 1;
    }

    return status;
}