# https://pypi.org/project/spidev/

import spidev
import time

BUS = 0
DEV = 1
spi = spidev.SpiDev()
spi.open(BUS, DEV)      # lihat di /dev/spidev<bus>.<device>

MSG_LENGTH = 5

# settings example
spi.max_speed_hz = 400000
spi.mode = 0b00

while True:
    data = spi.readbytes(5)
    print(data)
    time.sleep(3)

