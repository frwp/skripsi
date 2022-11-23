import smbus2 as smbus
import time

bus = smbus.SMBus()
SLAVE_ADDRESS = 0x04
MSG_LENGTH = 20          # 5 bytes of data expected

while True:
    try:
        rep = bus.read_i2c_block_data(SLAVE_ADDRESS, 0, MSG_LENGTH)
    except:
        continue
    payload = ""
    for i in range(MSG_LENGTH):     # Byte expected
        payload += chr(rep[i])
    print(payload)
    print(rep)
    time.sleep(2)
    