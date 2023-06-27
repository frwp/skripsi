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
            echo "Running main_spi at $START_TIME"
            ./main_spi >/dev/null 2>&1 &
            while true; do
                count=$(ps -aux | grep [m]ain_spi | wc -l)
                if [ $count -le 1 ]; then
                    break
                fi
                sleep 2
                ps -eo rss,pcpu,args |
                    (grep [m]ain_spi) |
                    awk '{ mem=$1/1000; printf("%5.2f,%5.2f,", mem, $2) } { for(x=3; x<=NF; x++) {printf("%s ",$x)} print "" }' \
                        >>result.spi.golang.$START_TIME.csv
            done
            ;;
        i2c)
            echo "Running main_i2c at $START_TIME"
            ./main_i2c >/dev/null 2>&1 &
            while true; do
                count=$(ps -aux | grep [m]ain_i2c | wc -l)
                if [ $count -le 1 ]; then
                    break
                fi
                sleep 2
                ps -eo rss,pcpu,args |
                    (grep [m]ain_i2c) |
                    awk '{ mem=$1/1000; printf("%5.2f,%5.2f,", mem, $2) } { for(x=3; x<=NF; x++) {printf("%s ",$x)} print "" }' \
                        >>result.i2c.golang.$START_TIME.csv
            done
            ;;
        *) usage ;;
        esac
    fi
}

run $@
