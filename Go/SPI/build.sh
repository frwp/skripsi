#!/bin/sh

echo "Building package..."
env GOARCH=arm GOARM=7 GOOS=linux go build -ldflags "-s -w" -a -o main_spi spi.go
mkdir -p target
mv main_spi target/main_spi
echo "Building done."

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
    rsync -P --rsh=ssh $(pwd)/target/main_spi ${working_host}:~/skripsi/Go/main_spi
else
    scp $(pwd)/target/main_spi ${working_host}:~/skripsi/Go/main_spi
fi

if [[ $? -ne 0 ]]; then
    echo "Transfer failed."
    return 1
else
    echo "Operation done."
fi
