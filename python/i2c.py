import traceback
import smbus2 as smbus
import time

bus = smbus.SMBus(1)
SLAVE_ADDRESS = 0x04
MSG_LENGTH = 5          # 5 bytes of data expected

while True:
    try:
        rep = bus.read_i2c_block_data(SLAVE_ADDRESS, 0, MSG_LENGTH)
    except:
        traceback.print_exception()
        exit(1)
    payload = ""
    for i in range(MSG_LENGTH):     # Byte expected
        payload += chr(rep[i])
    print(payload)
    time.sleep(0)