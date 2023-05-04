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
rpio.spiSetClockDivider(624);

let myInterval = setInterval(() => {
    try {
        var writeBuf = Buffer.alloc(58, 0x00);
        var readBuf = Buffer.alloc(writeBuf.length);
        writeBuf[0] = 0x10;
        rpio.spiTransfer(writeBuf, readBuf, 58);
        let data = readBuf.toString();
        let timeStamp = Math.floor(Date.now() / 1000);
        data = timeStamp + "|" + data;

        let formData = new URLSearchParams({
            data: data,
            node: "node1",
        });

        console.info(formData);

        axios
            .post("http://192.168.8.183/api", formData)
            .then((res) => {
                console.log(res.status);
            })
            .catch((err) => {
                console.error(err.data);
            });
    } catch (error) {
        console.error(error);
        rpio.spiEnd();
        rpio.close();
    }
}, 2000);


// run until 1 hour + 5 seconds (3605 seconds * 1000 ms)
setTimeout(() => {
    clearInterval(myInterval);
}, 3605000);
