/*******************************************************************************
 * spi.c
 * SPI master code @ Raspberry Pi in C language
 * @author Fransiskus Rian Wardana Putra
 * 3 Feb 2023
 * For detailed info, wiring, see README.md
 * build script is included in build.sh
 * Needs WiringPi library. See wiringpi.com and github.com/WiringPi
 */

#include <stdio.h>	// print
#include <unistd.h> // for sleep()
#include <stdlib.h> // for malloc() and free()
#include <wiringPiSPI.h>
#include <time.h>
#include <curl/curl.h>
#include <string.h>

#define SPI_CHANNEL 0
#define SPI_CLOCK_SPEED 400000
#define MAX_LENGTH 52

int main(void)
{
	char buf[1];				/* Buffer for SPI transfer, will be rewritten each time */
	char data[MAX_LENGTH];		/* For storing data */
	char *payload;				/* Storing final data */
	int seconds_since_epoch; 	/* Time unit since Epoch (UNIX Timestamp) */
	char time_str[10];			/* Storing char representation of time */
	int total_length = 0;		/* Total payload length */
	int fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_CLOCK_SPEED, 0);
	if (fd == -1)
	{
		puts("Failed to init SPI.");
		return -1;
	}
	puts("SPI communication successfully setup.");

	// initiate curl for http transfer
	CURL *curl;
	CURLcode res;
	curl_global_init(CURL_GLOBAL_ALL);

	struct timespec start, stop, sleep_dur, program_start; /* for measuring time elapsed */

	puts("Starting SPI transfer.");
	timespec_get(&program_start, TIME_UTC);
	while (stop.tv_sec - program_start.tv_sec < 3600)
	{
		timespec_get(&start, TIME_UTC);

		// Indicates starting point of each transaction.
		// if slave read 0x10, start SPI transfer at byte position 0.
		buf[0] = 0x10;
		wiringPiSPIDataRW(SPI_CHANNEL, buf, 1);

		for (int i = 0; i < MAX_LENGTH; i++) /* approximated data length */
		{
			// dummy byte, discarded on slave.
			buf[0] = 0x01;

			// exchange 1 byte with slave
			wiringPiSPIDataRW(SPI_CHANNEL, buf, 1);

			// buf[0] is populated with data from slave, replacing dummy byte
			data[i] = buf[0];
		}

		for (int i = 0; i < MAX_LENGTH; i++)
		{
			printf("%c", data[i]);
		}
		printf("\n");

		seconds_since_epoch = (int)time(NULL);

		// here we got 12 "magic number" from UNIX timestamp (time since epoch)
		// where time is in long int size which is 32 byte (2^32) and that number
		// in decimal is 10 char long. For char to be string, it needs to be
		// "NULL terminated", we end up with 11 byte long. 12 is a safe margin.
		total_length = strlen(data) + 12;
		payload = (char *)malloc(total_length);

		// convert time to time_str
		snprintf(time_str, 11, "%d", seconds_since_epoch);
		snprintf(payload, total_length, "%s|%s", time_str, data);
		puts(data);

		curl = curl_easy_init();
		if (curl)
		{	
			// init curl mime for multi-form post data
			curl_mime *mime = curl_mime_init(curl);
			
			// add first form field
			curl_mimepart *part = curl_mime_addpart(mime);

			// specify form name and data
			curl_mime_name(part, "node");
			curl_mime_data(part, "node1", CURL_ZERO_TERMINATED);

			// add second form field
			part = curl_mime_addpart(mime);

			// specify form name and data
			curl_mime_name(part, "data");
			curl_mime_data(part, payload, CURL_ZERO_TERMINATED);

			// set options used by curl
			// curl_easy_setopt(curl, CURLOPT_URL, "http://10.6.156.157/api");
			curl_easy_setopt(curl, CURLOPT_URL, "http://34.28.200.114/api");
			// curl_easy_setopt(curl, CURLOPT_POSTFIELDS, encoded_post_data);
			curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

			res = curl_easy_perform(curl);
			if (res != CURLE_OK)
			{
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			}
			// free allocated memory
			// curl_free(encoded_post_data);
			curl_free(mime);
			curl_free(part);
			free(payload);
			curl_easy_cleanup(curl);
		}

		timespec_get(&stop, TIME_UTC);

		sleep_dur.tv_sec = 2 - (stop.tv_sec - start.tv_sec);
		sleep_dur.tv_nsec = (stop.tv_nsec - start.tv_nsec) % 1000000000;

		nanosleep(&sleep_dur, NULL);
	}

	curl_global_cleanup();

	return 0;
}
