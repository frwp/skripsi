#!/bin/bash

function usage {
    echo "Usage: run.sh [args]"
    echo "       Valid args: spi, i2c (only one at a time)"
    return 1
}

function run {
    if [ $# -ne 1 ]; then
        usage
    else
        START_TIME=$(date +%d-%m-%Y:%T)
        case $1 in
        spi)
            echo "Running npm run spi at $START_TIME"
            npm run spi >/dev/null 2>&1 &
            while true; do
                count=$(ps -aux | grep [n]pm | wc -l)
                if [ $count -le 0 ]; then
                    sudo pkill node
                    break
                fi
                sleep 2
                ps -eo rss,pcpu,args |
                    (grep [n]pm) |
                    awk '{ mem=$1/1000; printf("%5.2f,%5.2f,", mem, $2) } { for(x=3; x<=NF; x++) {printf("%s ",$x)} print "" }' \
                        >>result.spi.javascript.$START_TIME.csv
            done
            ;;
        i2c)
            echo "Running npm run i2c at $START_TIME"
            npm run i2c >/dev/null 2>&1 &
            while true; do
                count=$(ps -aux | grep [n]pm | wc -l)
                if [ $count -le 0 ]; then
                    sudo pkill node
                    break
                fi
                sleep 2
                ps -eo rss,pcpu,args |
                    (grep [n]pm) |
                    awk '{ mem=$1/1000; printf("%5.2f,%5.2f,", mem, $2) } { for(x=3; x<=NF; x++) {printf("%s ",$x)} print "" }' \
                        >>result.i2c.javascript.$START_TIME.csv
            done
            ;;
        *) usage ;;
        esac
    fi
}

run $@
