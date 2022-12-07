#!/bin/sh

# Build docker image
echo "Building docker image..."
docker build . -t rust_cross_compile/armv7
echo "Building image done."

# Compile the app using created image
echo "Building package..."
docker run --rm -v $(pwd):/app rust_cross_compile/armv7
echo "Building package done."

host1="pi@192.168.8.141"
host2="pi@192.168.4.2"
check_connection="ssh -o ConnectTimeout=1 -q"

echo "Checking connection..."
if $($check_connection $host1 exit)
then
    working_host=$host1
    echo "Host ${host1} works"
elif $($check_connection $host2 exit)
then
    working_host=$host2
    echo "Host ${host2} works"
else
    echo "Connection failed, is the Raspberry Pi on or has the IP address changed?"
    exit 1
fi

echo ""
echo "Transfering executable to Raspberry Pi..."

if command -v rsync &> /dev/null
then
    rsync -P --rsh=ssh $(pwd)/target/armv7-unknown-linux-gnueabihf/release/main_spi ${working_host}:~/skripsi/Rust/main_spi
else
    scp $(pwd)/target/armv7-unknown-linux-gnueabihf/release/main_spi ${working_host}:~/skripsi/Rust/main_spi
fi

if [[ $? -ne 0 ]]; then
    echo "Transfer failed."
    return 1
else
    echo "Operation done."
fi
