# Repo untuk project skripsi Rian

Halo, namaku Fransiskus Rian Wardana Putra
repo ini untuk mengarsipkan skripsiku.

Readme ini dibuat biar ngga lupa aja apa yang jadi setting default di project ini.

Anyway, ini spek dasar:

| Var           | Value         |
| ---           | ---           |
| Freq          | 20,000,000*   |
| SLAVE_ADDRESS | 0x4           |
| MASTER_ADDRESS| doesn't matter|

> \* 20 MHz is theoretical value, as the chip will automatically tune it down to match max clock (16 MHz for ATMega328p 5V as Nano)

## Programming language for arduino
Of course use arduino IDE, why bother use other language, as arduino already use C++ base

## Programming language for raspi

| Language  | Library I2C   | Library SPI   |
| ---       | ---           | ---           |
| C         | Wiring.Pi     | Wiring.Pi     |
| Python    | smbus2        | spidev        |
| Go        | periph        | periph        |
| Rust      | RPPAL         | RPPAL         |
| C#        | .NET IoT      | .NET IoT      |
| Java      | Pi4J          | Pi4J          |
| Javascript| rpio          | rpio          |