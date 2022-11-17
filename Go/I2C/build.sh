#!/bin/sh

echo "Building package..."
env GOARCH=arm GOARM=7 GOOS=linux go build -a -o main_i2c i2c.go
echo "Building done."
echo "Transfering executable to Raspberry Pi..."
ssh -o ConnectTimeout=3 -q pi@192.168.4.2 exit || ssh -o ConnectTimeout=3 -q pi@192.168.8.141 exit
_R_CODE=$?
if [ $_R_CODE -ne 0 ]
then
    echo "Connection failed, is the Raspberry Pi on or has the IP address changed?"
    exit 1
else
    scp main_i2c pi@192.168.8.141:~/skripsi/Go/main_i2c
    echo "Operation done."
fi
