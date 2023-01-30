#!/bin/sh

host1="pi@192.168.8.141"
host2="pi@192.168.4.2"
host3="pi@10.6.178.201"
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
    rsync -P --rsh=ssh $(pwd)/{spi.py,i2c.py,requirements.txt} ${working_host}:~/skripsi/Python/
else
    scp $(pwd)/{spi.py,i2c.py,requirements.txt} ${working_host}:~/skripsi/Python/
fi
if [[ $? -ne 0 ]]; then
    echo "Transfer failed."
    exit 1
else
    echo "Operation done."
fi
