#!/bin/sh

echo "Building package..."
dotnet publish -c Release -r linux-arm --self-contained true
echo "Building done."


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
    rsync -P --rsh=ssh $(pwd)/bin/Release/net6.0/linux-arm/publish/* ${working_host}:~/skripsi/dotnet/spi/
else
    scp $(pwd)/bin/Release/net6.0/linux-arm/publish/* ${working_host}:~/skripsi/dotnet/spi/
fi

if [[ $? -ne 0 ]]; then
    echo "Transfer failed."
    return 1
else
    echo "Operation done."
fi
