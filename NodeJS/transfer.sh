#!/bin/sh

host1="pi@192.168.8.179"
host2="pi@192.168.4.2"
host3="pi@10.6.181.244"
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
echo "Transfer to Raspberry Pi..."

if command -v rsync &> /dev/null
then
    rsync -Pa --exclude=node_modules --rsh=ssh $(pwd)/* ${working_host}:~/skripsi/Js
else
    echo "Use rsync, otherwise node_modules are copied"
fi
if [[ $? -ne 0 ]]; then
    echo "Transfer failed."
    return 1
else
    echo "Operation done."
fi
