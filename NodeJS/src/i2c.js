var rpio = require("rpio");
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

setInterval(() => {
    try {
        var readBuf = Buffer.alloc(32, 0x00);
        var status = rpio.i2cRead(readBuf, 32);
        if (status !== RPIO.I2cStatusCode.OK) {
            let errcode = "";
            switch (status) {
                case RPIO.I2cStatusCode.ERROR_CLKT:
                    errcode = "clock stretch timeout";
                    break;
                case RPIO.I2cStatusCode.ERROR_DATA:
                    errcode = "not all data is sent/received";
                    break;
                case RPIO.I2cStatusCode.ERROR_NACK:
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

        let formData = {
            data,
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
        rpio.i2cEnd();
        rpio.close();
    }
}, 2000);
