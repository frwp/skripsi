import smbus2 as smbus
import time

bus = smbus.SMBus(1)
SLAVE_ADDRESS = 0x04

while True:
    try:
        rep = bus.read_i2c_block_data(SLAVE_ADDRESS, 0)
    except:
        continue

    payload = ""
    for i in range(39):
        payload += chr(rep[i])
    print(payload)
    time.sleep(1)