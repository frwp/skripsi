#!/bin/sh

# Build docker image
docker build . -t rust_cross_compile/armv7

# Compile the app using created image
docker run --rm -v $(pwd):/app rust_cross_compile/armv7

# if command -v rsync &> /dev/null
# then
#     rsync -P --rsh=ssh 
