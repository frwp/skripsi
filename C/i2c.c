#include <stdio.h>
#include <unistd.h>
#include <wiringPi.h>
#include <wiringPiI2C.h>

int main(void)
{
    printf("I2C test\n");

    const int SLAVE_ADDRESS = 0x04;
    const int MESSAGE_LENGTH = 5;

    // Initiate wiringPi
    int errno = wiringPiSetup();
    if (errno < 0)
    {
        printf("setup wiringPi failed: errno %d\n", errno);
        return errno;
    }
    puts("wiringPi init success");

    // Initiate i2c bus; gets linux file descriptor (fd)
    int fd = wiringPiI2CSetup(SLAVE_ADDRESS);
    if (fd < 0)
    {
        printf("setup wiringPi I2C failed: errno %d\n", fd);
        return fd;
    }
    puts("i2c init success");

    for (;;)
    {
        // Initiate buffer
        char buf[MESSAGE_LENGTH];

        // read the message into the buffer
        // using linux' read command because wiringpi doesn't support
        // multiple bytes read and write
        // see `man 2 read`
        read(fd, buf, MESSAGE_LENGTH);

        // print buffer
        for (int i = 0; i < MESSAGE_LENGTH; i++)
        {
            printf("%c", buf[i]);
        }
        if (buf[0])
            printf("\n");
        sleep(2);
    }
    return 0;
}