# https://pypi.org/project/spidev/

import spidev
import time

BUS = 0
DEV = 0
spi = spidev.SpiDev()
spi.open(BUS, DEV)      # lihat di /dev/spidev<bus>.<device>

MESSAGE_LENGTH = 42

# settings, SPI mode 0, max speed 400kHz
spi.max_speed_hz = 400000
spi.mode = 0b00

try:
    while True:
        start = time.time_ns()

        # placeholder
        # sent data length == expected data length received from slave
        buf = [0x00] * MESSAGE_LENGTH
        buf[0] = 0x10

        data = spi.xfer(buf)

        # Convert returned data to char, and joins them to a string
        data = "".join([chr(x) for x in data])

        print(data)
        time_diff = time.time_ns() - start

        # time diff is in nanosecond, convert to second but retaining floating points
        time_diff /= 1000000000
        time.sleep(2 - time_diff)

finally:
    # close spi device before exit
    spi.close()