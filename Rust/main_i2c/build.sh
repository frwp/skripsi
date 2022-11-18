#!/bin/sh

# we need to install cross build gcc dependency
# install gcc-arm-linux-gnu

readonly  TARGET_ARCH=armv7-unknown-linux-gnueabihf

cargo build --target=${TARGET_ARCH}