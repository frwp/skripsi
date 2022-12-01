# Arduino readme

## Komponen
1. Arduino Nano (versi colokan pakai USB-C)
2. GY-521 MPU6050 (accelerometer dan gyroscope)
3. DHT22 (temperature and humidity)
4. LED (ngga terlalu perlu, untuk debug apakah code berjalan atau tidak karena nantinya pin arduino LED_BUILTIN (D13) dipakai untuk SCLK di SPI)

---
## Konektivitas
Ini yang basic, I2C dan SPI pakai skema ini untuk menyambungkan sensor dengan arduino
| Ardunio   | DHT22 | GY-521    | LED                   |
| ---       | ---   | ---       | ---                   |
| 5V        | VIN   | VCC       | -                     |
| GND       | GND   | GND       | GND (-) (yang pendek) |
| D7        | OUT   | -         | -                     |
| A4        | -     | SDA       | -                     |
| A5        | -     | SCL       | -                     |
| D2        | -     | INT       | -                     |
| A1        | -     | -         | R->(+) (yang panjang) |

## Library
```
DHT.h   ->  DHT22
DHT_U.h ->

I2Cdev.h                        -> GY-521 MPU6050
MPU6050_6Axis_MotionApps20.h    ->
```
