FROM rust:slim-bullseye

RUN apt update && apt upgrade -y
RUN apt install -y gcc-arm-linux-gnueabihf libc6-dev-armhf-cross

RUN rustup target add armv7-unknown-linux-gnueabihf
# RUN rustup toolchain install stable-armv7-unknown-linux-gnueabihf

WORKDIR /app

ENV CARGO_TARGET_ARMV7_UNKNOWN_LINUX_GNUEABIHF_LINKER=arm-linux-gnueabihf-gcc \
        CC_armv7_unknown_Linux_gnueabihf=arm-linux-gnueabihf-gcc \
        CARGO_HOME=/app/cargo_home

CMD ["cargo", "build", "--release", "--target", "armv7-unknown-linux-gnueabihf"]
