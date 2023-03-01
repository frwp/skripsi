// #include <wiringPiSPI.h>
#include <stdio.h>
#include <curl/curl.h>

#define SPI_CHANNEL 0
#define SPI_CLOCK_SPEED 400000
#define MAX_LENGTH 54

int main(void)
{
    unsigned char buf[1];
    unsigned char data[MAX_LENGTH];
    int fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_CLOCK_SPEED, 0);
    if (fd == -1)
    {
        puts("Failed to init SPI.");
        return -1;
    }
    puts("SPI communication successfully setup.");

    CURL *curl;
    CURLcode res;
    curl_global_init(CURL_GLOBAL_ALL);

    struct timespec start, stop, sleep_dur; /* for measuring time elapsed */

    puts("Starting SPI transfer.");
    for (;;)
    {
        timespec_get(&start, TIME_UTC);

        buf[0] = 0x10;
        wiringPiSPIDataRW(SPI_CHANNEL, buf, 1);

        for (int i = 0; i < MAX_LENGTH; i++)
        {
            buf[0] = 0x01;
            wiringPiSPIDataRW(SPI_CHANNEL, buf, 1);
            data[i] = buf[0];
        }

        for (int i = 0; i < MAX_LENGTH; i++)
        {
            printf("%c", data[i]);
        }
        printf("\n");

        curl = curl_easy_init();
        if (curl)
        {
            curl_easy_setopt(curl, CURLOPT_URL, "http://34.28.200.114/api");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "node=node1&data=data");
        }

        timespec_get(&stop, TIME_UTC);
        sleep_dur.tv_sec = 2 - (stop.tv_sec - start.tv_sec);
        sleep_dur.tv_nsec = (stop.tv_nsec - start.tv_nsec) % 1000000000;
        nanosleep(&sleep_dur, NULL);
    }

    return 0;
}
