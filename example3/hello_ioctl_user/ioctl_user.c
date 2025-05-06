#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

#define DEVICE_PATH "/dev/ioctl_example"
#define IOCTL_MAGIC 'k'
#define IOCTL_CMD_READ  _IOR(IOCTL_MAGIC, 0, unsigned long)
#define IOCTL_CMD_WRITE _IOW(IOCTL_MAGIC, 1, unsigned long)
#define IOCTL_CMD_RDWR  _IOWR(IOCTL_MAGIC, 2, unsigned long)
#define IOCTL_CMD_NONE  _IO(IOCTL_MAGIC, 3)

int main() {
    int fd = open(DEVICE_PATH, O_RDWR);
    unsigned long user_data;
    char buffer[1024];

    if(fd < 0) {
        perror("Failed to open the device.");
        return EXIT_FAILURE;
    }

    // Test IOCTL_CMD_NONE
    if(ioctl(fd, IOCTL_CMD_NONE) < 0) {
        perror("ioctl NONE failed");
    } else {
        printf("Successfully executed IOCTL_CMD_NONE\n");
    }

    close(fd);
    return EXIT_SUCCESS;
}