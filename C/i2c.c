/*******************************************************************************
 * i2c.c
 * I2C master code @ Raspberry Pi in C language
 * @author Fransiskus Rian Wardana Putra
 * 28 Oct 2022
 *
 * Kode ini digunakan untuk tugas akhir S1 Ilmu Komputer dengan topik
 * Bahasa pemrograman dan protokol komunikasi untuk IoT
 *
 * Kode ini menggunakan library WiringPi untuk melakukan setup ke hardware I2C
 * pada perangkat Raspberry Pi. Lihat wiringpi.com dan github.com/WiringPi
 *
 * Perangkat dihubungkan sebagai berikut:
 * (Raspberry Pi)   (Arduino Nano)
 * GND          ->  GND
 * 5V           ->  5V (VCC)
 * SDA          ->  A4 (SDA) (dengan pull-up resistor 4.7K ke VCC)
 * SCL          ->  A5 (SCL) (dengan pull-up resistor 4.7K ke VCC)
 *
 * To build this file, run:
 * > gcc i2c.c -lwiringpi -o i2c
 */

#include <stdio.h>  // print
#include <stdlib.h> // exit()
#include <unistd.h> // For sleep() and read()
#include <wiringPi.h>
#include <wiringPiI2C.h>

int main(void)
{
    puts("I2C test");

    const int SLAVE_ADDRESS = 0x04;
    const int MESSAGE_LENGTH = 32;

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

        printf("%s", buf);
        if (buf[0])
            printf("\n");
        if (!buf[0])
        {
            puts("empty");
            exit(1);
        }
        sleep(2);
    }
    return 0;
}