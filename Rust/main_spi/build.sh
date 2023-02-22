#!/bin/sh

# # Build docker image
# echo "Building docker image..."
# docker build . -t rust_cross_compile/armv7
# if [[ $? -ne 0 ]]; then
#     echo "Docker build failed."
#     exit 1
# fi
# echo "Building image done."

# # Compile the app using created image
# echo "Building package..."
# docker run --rm -v $(pwd):/app rust_cross_compile/armv7
# if [[ $? -ne 0 ]]; then
#     echo "Building package failed."
#     exit 1
# echo "Building package done."

# Docker build doesn't work (at least not as easy as below)
# Use cross crate for cross compiling => https://github.com/cross-rs/cross
# cross uses the same command and options as cargo for building and running
# still uses Docker underneath, follow instruction on installing cross on the docs
cross build --release --target armv7-unknown-linux-gnueabihf

if [[ $? -ne 0 ]]; then
    echo "Compile failed."
    exit 1
else
    echo "Compile done."
fi

host1="pi@192.168.8.141"
host2="pi@192.168.4.2"
host3="pi@10.6.174.146"
check_connection="ssh -o ConnectTimeout=1 -q"

echo "Checking connection..."
for host in $host1 $host2 $host3
    do
        if $($check_connection $host exit)
        then
            working_host=$host
        fi
    done
if [ -v working_host ]
then
    echo "Host ${working_host} works"
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
    exit 1
else
    echo "Operation done."
fi
