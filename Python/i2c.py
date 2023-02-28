# import traceback
from smbus2 import SMBus, i2c_msg
import time
import httpx
import asyncio
from math import floor

bus = SMBus(1)
SLAVE_ADDRESS = 0x04
MSG_LENGTH = 32          # bytes of data expected

async def send_requests(client: httpx.AsyncClient, url: str, payload):
    await client.post(url, data=payload)

def main():
    timer = time.time()
    try:
        client = httpx.AsyncClient(base_url='http://34.28.200.114')
        while time.time() - timer < 3600:
            start = time.time_ns()
            try:
                read = i2c_msg.read(SLAVE_ADDRESS, MSG_LENGTH)
                bus.i2c_rdwr(read)
                rep = list(read)
                data = ''.join(chr(i) for i in rep).strip()
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

            duration = time.time_ns() - start
            duration /= 1e9
            if duration > 2:
                continue
            time.sleep(2 - duration/1e9)
    finally:
        # close bus before exit
        bus.close()
        client.aclose()

main()