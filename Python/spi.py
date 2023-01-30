# https://pypi.org/project/spidev/

import spidev
import time
import httpx
import asyncio
from math import floor

BUS = 0
DEV = 0
spi = spidev.SpiDev()
spi.open(BUS, DEV)      # lihat di /dev/spidev<bus>.<device>

MESSAGE_LENGTH = 58

# settings, SPI mode 0, max speed 400kHz
spi.max_speed_hz = 400000
spi.mode = 0b00

async def send_requests(client: httpx.AsyncClient, url: str, payload):
    await client.post(url, data=payload)

def main():
    count = 0
    client = httpx.AsyncClient(base_url='http://34.28.200.114')
    try:
        while count < 310: # change accordingly
            start = time.time_ns()

            # placeholder
            # sent data length == expected data length received from slave
            buf = [0x00] * MESSAGE_LENGTH
            buf[0] = 0x10

            try:
                data = spi.xfer(buf)
                # Convert returned data to char, and joins them to a string
                data = "".join([chr(x) for x in data]).strip()
                if len(data) < 24:
                    raise Exception('message incomplete')
                data = str(floor(time.time())) + "|" + data
            except KeyboardInterrupt:
                break
            except Exception as e:
                print(e)
                continue
            print(data)

            payload = {'data': data, 'node': 'node1'}
            loop = asyncio.get_event_loop()
            try:
                loop.run_until_complete(send_requests(client, '/api', payload))
            except KeyboardInterrupt:
                break
            except Exception as e:
                print(e)
                continue

            time_diff = time.time_ns() - start

            # time diff is in nanosecond, convert to second but retaining floating points
            time_diff /= 1e9
            count += 1
            time.sleep(2 - time_diff)
    finally:
        # close spi device before exit
        spi.close()
        client.aclose()

main()