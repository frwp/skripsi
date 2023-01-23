var rpio = require("rpio");
const axios = require("axios");

var options = {
    gpiomem: false,
    mapping: "gpio",
    mock: undefined,
    close_on_exit: true,
};

rpio.init(options);
rpio.spiBegin();
rpio.spiChipSelect(0);
rpio.spiSetClockDivider(625);

setInterval(() => {
    try {
        var writeBuf = Buffer.alloc(58, 0x00);
        var readBuf = Buffer.alloc(writeBuf.length);
        writeBuf[0] = 0x10;
        rpio.spiTransfer(writeBuf, readBuf, 58);
        let data = readBuf.toString();
        let timeStamp = Math.floor(Date.now() / 1000);
        data = timeStamp + "|" + data;

        let formData = {
            data: data,
            node: "node1",
        };

        console.info(formData);

        axios
            .post("http://192.168.8.134:8080/api", formData)
            .then((res) => {
                console.info(res);
            })
            .catch((err) => {
                console.error(err);
            });
    } catch (error) {
        console.error(error);
    } finally {
        rpio.spiEnd();
        rpio.close();
    }
}, 2000);
