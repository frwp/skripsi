var rpio = require("rpio");
const axios = require("axios");
const i2cSlaveAddress = 0x04;

var options = {
    gpiomem: false,
    mapping: "gpio",
    mock: undefined,
    close_on_exit: true,
};

rpio.init(options);
rpio.i2cBegin();
rpio.i2cSetSlaveAddress(i2cSlaveAddress);
rpio.i2cSetBaudRate(400000);

let myInterval = setInterval(() => {
    try {
        var readBuf = Buffer.alloc(32, 0x00);
        var status = rpio.i2cRead(readBuf, 32);
        if (status !== 0x00 /** RPIO.I2cStatusCode.OK */) {
            let errcode = "";
            switch (status) {
                case 0x02 /** RPIO.I2cStatusCode.ERROR_CLKT */:
                    errcode = "clock stretch timeout";
                    break;
                case 0x04 /** RPIO.I2cStatusCode.ERROR_DATA */:
                    errcode = "not all data is sent/received";
                    break;
                case 0x01 /** RPIO.I2cStatusCode.NACK */:
                    errcode = "received NACK";
                    break;
                default:
                    break;
            }
            console.error(status + ": " + errcode);
        }

        let data = readBuf.toString();
        let timeStamp = Math.floor(Date.now() / 1000);
        data = timeStamp + "|" + data;

        let formData = new URLSearchParams({
            data,
            node: "node1",
        });

        console.info(formData);

        axios
            .post("http://192.168.8.185/api", formData)
            .then((res) => {
                console.info(res.status);
            })
            .catch((err) => {
                console.error(err.data);
            });
    } catch (error) {
        console.error(error);
        rpio.i2cEnd();
        rpio.close();
    }
}, 2000);

// run until 1 hour + 5 seconds (3605 seconds * 1000 ms)
setTimeout(() => {
    clearInterval(myInterval);
}, 3605000);
