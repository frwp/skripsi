/*******************************************************************************
 * spi.c
 * SPI master code @ Raspberry Pi in C language
 * @author Fransiskus Rian Wardana Putra
 * 2 Dec 2022
 * For detailed info, wiring, see README.md
 * build script is included in build.sh
 * Needs WiringPi library. See wiringpi.com and github.com/WiringPi
*/

#include <stdio.h>	// print
#include <unistd.h> // for sleep()
#include <wiringPiSPI.h>
#include <time.h>

#define SPI_CHANNEL 0
#define SPI_CLOCK_SPEED 400000
#define DATA_LENGTH 58

int main(void)
{
	unsigned char buf[1];
	unsigned char data[50];
	int fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_CLOCK_SPEED, 0);
	if (fd == -1)
	{
		puts("Failed to init SPI.");
		return -1;
	}
	puts("SPI communication successfully setup.");

	struct timespec start, stop, sleep_dur; /* for measuring time elapsed */

	puts("Starting SPI transfer.");
	for (;;)
	{
		timespec_get(&start, TIME_UTC);

		// Indicates starting point of each transaction.
		// if slave read 0x10, start SPI transfer at byte position 0.
		buf[0] = 0x10;
		wiringPiSPIDataRW(SPI_CHANNEL, buf, 1);

		for (int i = 0; i < MESSAGE_LENGTH; i++) /* 42 is approximated data length */
		{
			// dummy byte, discarded on slave.
			buf[0] = 0x01;

			// exchange 1 byte with slave
			wiringPiSPIDataRW(SPI_CHANNEL, buf, 1);

			// buf[0] is populated with data from slave, replacing dummy byte
			data[i] = buf[0];
		}

		for (int i = 0; i < MESSAGE_LENGTH; i++)
		{
			printf("%c", data[i]);
		}
		printf("\n");
		timespec_get(&stop, TIME_UTC);

		sleep_dur.tv_sec = 2 - (stop.tv_sec - start.tv_sec);
		sleep_dur.tv_nsec = (stop.tv_nsec - start.tv_nsec) % 1000000000;

		nanosleep(&sleep_dur, NULL);
	}

	return 0;
}
