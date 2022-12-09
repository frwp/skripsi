var rpio = require("rpio");

var options = {
    gpiomem: false,
    mapping: "gpio",
    mock: undefined,
    close_on_exit: true,
};

rpio.init(options);
rpio.spiBegin();
rpio.spiChipSelect(0);
rpio.spiSetClockDivider(624);

setInterval(() => {
    try {
        var writeBuf = Buffer.alloc(58, 0x00);
        var readBuf = Buffer.alloc(writeBuf.length);
        writeBuf[0] = 0x10;
        rpio.spiTransfer(writeBuf, readBuf, 58);
        console.info("test");
    } catch (error) {
        rpio.spiEnd();
        rpio.close();
    }
}, 2000);
