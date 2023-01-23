#!/bin/sh

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
echo "Transfer to Raspberry Pi..."

if command -v rsync &> /dev/null
then
    rsync -Pa --exclude=node_modules --rsh=ssh $(pwd)/* ${working_host}:~/skripsi/Js
else
    # scp $(pwd)/spi.py ${working_host}:~/skripsi/Python/spi.py
    echo "Use rsync, otherwise node_modules are copied"
fi
if [[ $? -ne 0 ]]; then
    echo "Transfer failed."
    return 1
else
    echo "Operation done."
fi
