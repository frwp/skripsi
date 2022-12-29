# import traceback
from smbus2 import SMBus, i2c_msg
import time

bus = SMBus(1)
SLAVE_ADDRESS = 0x04
MSG_LENGTH = 32          # bytes of data expected

while True:
    start = time.time_ns()
    try:
        read = i2c_msg.read(SLAVE_ADDRESS, MSG_LENGTH)
        bus.i2c_rdwr(read)
        rep = list(read)
    except Exception as e:
        # traceback.print_exc(e)
        continue
    duration = time.time_ns() - start
    payload = ''.join(chr(i) for i in rep)
    print(payload)
    # print(rep)
    time.sleep(2 - duration/1e9)
