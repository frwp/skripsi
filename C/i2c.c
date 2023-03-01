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
#include <time.h>
#include <curl/curl.h>
#include <string.h>

int main(void)
{
    puts("I2C test");

    const int SLAVE_ADDRESS = 0x04;
    const int MESSAGE_LENGTH = 32;

    // Initiate variables
    char buf[MESSAGE_LENGTH]; /* Storing data */
    char *payload;            /* Storing final data */
    int seconds_since_epoch;  /* Time unit since Epoch (UNIX Timestamp) */
    char time_str[10];        /* Storing char representation of time */
    int total_length = 0;     /* Total payload length */

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

    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);

    struct timespec start, stop, sleep_dur, program_start; /* for measuring time elapsed */

    puts("Starting I2C transfer.");
    timespec_get(&program_start, TIME_UTC);

    while (stop.tv_sec - program_start.tv_sec < 5)
    {
        timespec_get(&start, TIME_UTC);

        // read the message into the buffer
        // using linux' read command because wiringpi doesn't support
        // multiple bytes read and write
        // see `man 2 read`
        read(fd, buf, MESSAGE_LENGTH);

        printf("%s\n", buf);

        seconds_since_epoch = (int)time(NULL);

        // here we got 12 "magic number" from UNIX timestamp (time since epoch)
        // where time is in long int size which is 32 byte (2^32) and that number
        // in decimal is 10 char long. For char to be string, it needs to be
        // "NULL terminated", we end up with 11 byte long. 12 is a safe margin.
        total_length = strlen(data) + 12;
        payload = (char *)malloc(total_length);

        // convert time to time_str
        snprintf(time_str, 11, "%d", seconds_since_epoch);
        snprintf(payload, total_length, "%s|%s", time_str, buf);
        puts(payload);

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