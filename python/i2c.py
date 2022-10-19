import smbus2 as smbus
import time

bus = smbus.SMBus(1)
SLAVE_ADDRESS = 0x04
MSG_LENGTH = 5          # 5 bytes of data expected

while True:
    rep = bus.read_i2c_block_data(SLAVE_ADDRESS, 0, MSG_LENGTH)

    payload = ""
    for i in range(5):     # Byte expected
        payload += chr(rep[i])
    print(payload)
    time.sleep(1)