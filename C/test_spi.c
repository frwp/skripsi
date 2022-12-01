#include <wiringPiSPI.h>
#include <stdio.h>

#define SPI_CHANNEL 0
#define SPI_CLOCK_SPEED 400000

int main(void)
{
    unsigned char buf[1];
    unsigned char data[100];
    int fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_CLOCK_SPEED, 0);
    if (fd == -1) {
        puts("Failed to init SPI.");
        return -1;
    }
    puts("SPI communication successfully setup.");

    buf[0] = 0x10;
    wiringPiSPIDataRW(SPI_CHANNEL, buf, 1);
    puts("Starting SPI transfer.");

    for (int i = 0; i < 42; i++) {
        buf[0] = 0x01;
        wiringPiSPIDataRW(SPI_CHANNEL, buf, 1);
	data[i] = buf[0];
    }

    for (int i = 0; i < 42; i++) {
        printf("%c", data[i]);
    }
    printf("\n");

    return 0;
}
