#!/bin/sh

flags="-Os"
while getopts ':dh' OPTION; do
	case "$OPTION" in
		d) flags="-g -Og "
		   ;;

		*) echo "Usage ./build.sh [-d]"
		   echo " "
		   echo "Options:"
		   echo "-d : activate debug flags"
		   exit 1
		   ;;
	esac
done
# use this command if debugging
gcc spi.c ${flags} -lwiringPi $(curl-config --libs) -o main_spi