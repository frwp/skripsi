# Pseudocode for I2C on Raspberry Pi

```sh
# Initialize variables and constants
MESSAGE_LENGTH  := int                  # varies depends on the payload

# Initialize library
obj_lib := library_constructor_call()

# Main loop
while True:
    
    # get current time in ms
    start_time := time_in_ms()

    read_buffer     := [] * MESSAGE_LENGTH  # empty read buffer
    write_buffer    := [] * MESSAGE_LENGTH  # empty write buffer
    
    # fill first byte with "contract code" so the arduino recognize the other device
    write_buffer[0] = 0x10

    # get the buffer from arduino using library call
    buffer := obj_lib.transfer_full_duplex(write_buffer, read_buffer)
    
    # calculate time needed for read call
    total_time := time_in_ms() - start_time

    # error check if read exceed 2 seconds, exit program
    if total_time > 2000:
        exit(1)

    # convert buffer bytes to string
    for i in [ 0 .. MESSAGE_LENGTH ]:
        payload = payload + char(buffer[i])
    
    # process payload (send to server)
    http_push(URL, payload)

    # sleep until total loop time is 2 seconds
    time.sleep(2000 - total_time)
```