# Arduino readme

## Komponen
1. Arduino Nano (versi colokan pakai USB-C)
2. GY-521 MPU6050 (accelerometer dan gyroscope)
3. DHT22 (temperature and humidity)

---
## Konektivitas
| Ardunio   | GY-521    | DHT22 |
| ---       | ---       | ---   |
| 5V        | VIN       | VCC   |
| GND       | GND       | GND   |
| A4        | -         | SDA   |
| A5        | -         | SCL   |
| D7        | OUT       | -     |

## Library
```
DHT.h   ->  DHT22
DHT_U.h ->

I2Cdev.h                        -> GY-521 MPU6050
MPU6050_6Axis_MotionApps20.h    ->
```
