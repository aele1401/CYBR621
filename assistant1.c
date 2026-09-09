#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#define BUFFER_SIZE 4096

static int write_all(int fd, const char *buf, ssize_t count)
{
    ssize_t total = 0;

    while (total < count) {
        ssize_t written = write(fd, buf + total, count - total);

        if (written < 0) {
            if (errno == EINTR)
                continue;

            return -1;
        }

        if (written == 0)
            return -1;

        total += written;
    }

    return 0;
}

int main(int argc, char *argv[])
{
    int src_fd = -1;
    int dst_fd = -1;
    char buffer[BUFFER_SIZE];

    /* Validate command-line arguments. */
    if (argc != 3)
        return 1;

    /* Open source file read-only. */
    src_fd = open(argv[1], O_RDONLY);
    if (src_fd < 0)
        return 1;

    /*
     * Open destination with:
     * O_WRONLY  - write-only
     * O_CREAT   - create if it does not exist
     * O_EXCL    - fail if it already exists
     *
     * 0600 means read/write for owner only.
     */
    dst_fd = open(argv[2], O_WRONLY | O_CREAT | O_EXCL, 0600);

    if (dst_fd < 0) {
        close(src_fd);
        return 1;
    }

    /* Copy the file. */
    for (;;) {
        ssize_t bytes_read;

        bytes_read = read(src_fd, buffer, sizeof(buffer));

        if (bytes_read < 0) {
            if (errno == EINTR)
                continue;

            close(dst_fd);
            close(src_fd);
            return 1;
        }

        /* End of file. */
        if (bytes_read == 0)
            break;

        /* Handle partial writes. */
        if (write_all(dst_fd, buffer, bytes_read) < 0) {
            close(dst_fd);
            close(src_fd);
            return 1;
        }
    }

    /* Close both file descriptors before exiting. */
    if (close(dst_fd) < 0) {
        close(src_fd);
        return 1;
    }

    if (close(src_fd) < 0)
        return 1;

    return 0;
}