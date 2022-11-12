/*******************************************************************************
 * spi.c
 * SPI master code @ Raspberry Pi in C language
 * @author Fransiskus Rian Wardana Putra
 * 28 Oct 2022
 *
 * Kode ini digunakan untuk tugas akhir S1 Ilmu Komputer dengan topik
 * Bahasa pemrograman dan protokol komunikasi untuk IoT
 *
 * Kode ini menggunakan library WiringPi untuk melakukan setup ke hardware SPI
 * pada perangkat Raspberry Pi. Lihat wiringpi.com dan github.com/WiringPi
 *
 * Perangkat dihubungkan sebagai berikut:
 * (Raspberry Pi)   (Arduino Nano)
 * GND          ->  GND
 * 5V           ->  5V
 * CE0          ->  D10 (SS)
 * MOSI         ->  D11 (COPI)
 * MISO         ->  D12 (CIPO)
 *
 * To build this file, run:
 * > gcc spi.c -lwiringpi -o spi
 */

#include <stdio.h>	// print
#include <stdlib.h> // exit()
#include <unistd.h> // sleep() and read()
#include <wiringPi.h>
#include <wiringPiSPI.h>

int main(void)
{
	puts("SPI test");

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

	// Initiate spi bus; returns linux file descriptor (fd)
	int fd = wiringPiSPISetup(SLAVE_ADDRESS);
	if (fd < 0)
	{
		printf("setup wiringPi SPI failed: errno %d\n", fd);
		return fd;
	}
	puts("SPI init success");

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

		return 0;
	}
