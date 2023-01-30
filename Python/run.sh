#! /bin/sh

function usage {
    echo "Usage: run.sh [args]"
    echo "       Valid args: spi, i2c (only one at a time)"
    return 1
}

function run {
    if [ $# -ne 1 ]
    then
        usage
    else
        START_TIME=$(date +%d-%m-%Y:%T)
        case $1 in
        spi|spi.py) echo "Running spi.py at $START_TIME";
                    python3 spi.py > /dev/null 2>&1 &
                    while true; do
                        count=$(ps -aux | grep spi.py | wc -l)
                        if [ $count -le 1 ];
                        then
                            break
                        fi
                        sleep 2
                        ps -eo rss,pcpu,args | grep [s]pi.py | awk '{ mem=$1/1000; printf("%5.2f,%5.2f,", mem, $2) } { for(x=3; x<=NF; x++) {printf("%s",$x)} print "" }' >> result.spi.python.$START_TIME.csv
                    done
                    ;;
        i2c|i2c.py) echo "i2c";
                    echo "i2c2";
                    echo "i2c3";;
        *) usage ;;
        esac
    fi
}

run $@