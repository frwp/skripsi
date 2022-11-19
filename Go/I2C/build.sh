#!/bin/sh

echo "Building package..."
env GOARCH=arm GOARM=7 GOOS=linux go build -ldflags "-s -w" -a -o main_i2c i2c.go
mkdir -p target
mv main_i2c target/main_i2c
echo "Building done."

echo "Transfering executable to Raspberry Pi..."
ssh -o ConnectTimeout=1 -q pi@192.168.4.2 exit || ssh -o ConnectTimeout=1 -q pi@192.168.8.141 exit
if [ $? -ne 0 ]
then
    echo "Connection failed, is the Raspberry Pi on or has the IP address changed?"
    exit 1
else
    if command -v rsync &> /dev/null
    then
        rsync -P --rsh=ssh $(pwd)/target/main_i2c pi@192.168.8.141:~/skripsi/Go/main_i2c
    else
        scp $(pwd)/target/main_i2c pi@192.168.8.141:~/skripsi/Go/main_i2c
    fi
    if [ $? -ne 0 ]; then
        echo "Transfer failed."
        return 1
    else
        echo "Operation done."
    fi
fi
