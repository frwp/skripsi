#!/bin/sh

# TODO: adjust for spi folder
# Build docker image
echo "Building docker image..."
docker build . -t rust_cross_compile/armv7
echo "Building image done."

# Compile the app using created image
echo "Building package..."
docker run --rm -v $(pwd):/app rust_cross_compile/armv7
echo "Building package done."

echo "Transfering executable to Raspberry Pi..."
ssh -o ConnectTimeout=1 -q pi@192.168.4.2 exit || ssh -o ConnectTimeout=1 -q pi@192.168.8.141 exit
if [ $? -ne 0 ]
then
    echo "Connection failed, is the Raspberry Pi on or has the IP address changed?"
    exit 1
else
    if command -v rsync &> /dev/null
    then
        rsync -P --rsh=ssh $(pwd)/target/armv7-unknown-linux-gnueabihf/release/main_spi pi@192.168.8.141:~/skripsi/Rust/main_spi
    else
        scp $(pwd)/target/armv7-unknown-linux-gnueabihf/release/main_spi pi@192.168.8.141:~/skripsi/Rust/main_spi
    fi
    echo "Operation done."
fi
