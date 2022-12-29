# Repo untuk project skripsi Rian

Halo, namaku Fransiskus Rian Wardana Putra\
repo ini untuk mengarsipkan skripsiku.

Readme ini dibuat biar ngga lupa aja apa yang jadi setting default di project ini.\
Anyway, ini spek dasar:
| Var           | Value         |
| ---           | ---           |
| Freq          | 20,000,000*   |
| SLAVE_ADDRESS | 0x4           |
| MASTER_ADDRESS| doesn't matter|

> \* 20 MHz is theoretical value, as the chip will automatically tune it down to match max clock (16 MHz for ATMega328p 5V as Nano)

## Routing kabel
 Perangkat dihubungkan sebagai berikut:
### I2C
```
(Raspberry Pi)      (Arduino Nano)
GND             ->  GND
5V              ->  5V (VCC)
3 (SDA)         ->  A4 (SDA) (dengan pull-up resistor 4.7K ke VCC)
5 (SCL)         ->  A5 (SCL) (dengan pull-up resistor 4.7K ke VCC)
 ```
 
 ### SPI
```
(Raspberry Pi)      (Arduino Nano)
GND             ->  GND
5V              ->  5V (VCC)
MISO            ->  D12 (CIPO)
MOSI            ->  D11 (COPI)
SCLK            ->  D13 (SCK)
CE0             ->  D10 (SS)
```

## Programming language for arduino
Of course use arduino IDE, why bother use other language, as arduino already use C++ base

Arduino file used for I2C is test_analog, because I can only make the device works using
ADXL335 sensor, which uses analog interface, and not MPU6050.

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

## Bagaimana cara memilih bahasa pemrograman tersebut?
Dari survei stackoverflow tahun 2022, ambil top 10 programming language (shell tidak termasuk)\
Didapatkan list berikut:

| Most Popular      | Most loved        | most wanted       |
| ---               | ---               | ---               |
| _Javascript_      | **Rust**          | **Rust**          |
| **Python**        | Elixir            | **Python**        |
| **Typescript**    | Clojure           | **Typescript**    |
| _Java_            | **Typescript**    | **Go**            |
| **C#**            | Julia             | _Javascript_      |
| _C++_             | **Python**        | _Kotlin_          |
| PHP               | Delphi            | _C++_             |
| C                 | **Go**            | **C#**            |
| **Go**            | **C#**            | _Java_            |
| **Rust**          | _Kotlin_          | Dart              |

Ambil bahasa yang ada di minimal 2 kolom tabel tersebut.\
Didapatkan daftar berikut:
| Bahasa        | Skor |
| ---           | ---  |
| Python        | 3    |
| Typescript    | 3    |
| C#            | 3    |
| Go            | 3    |
| Rust          | 3    |
| Javascript    | 2    |
| C++           | 2    |
| Java          | 2    |
| Kotlin        | 2    |

---
## QnA
> Kenapa Javascript masuk dan bukan Typescript?

Kode typescript kita di-compile ke javascript, sehingga kode javascript yang baik === kode typescript.

> Kenapa Java masuk tapi kotlin tidak?

Mirip seperti kasus javascript, tetapi di sini yang sama adalah penggunaan JVM dan library Pi4J.

> Kenapa memaksakan C padahal C tidak dicintai, justru C++ ditinggalkan

C++ menambah kompleksitas pemrograman ~~dan ukuran dari binary yang dihasilkan karena stdlib.~~\
Selain itu, kalau fungsional program kita hanya sederhana (tidak pakai class/OOP dan fitur C++ lain),\
sama saja kita menulis C. Kenapa tidak sekalian C saja?

---
Kemudian mulailah perjalanan mencari library yang bisa akses pin gpio dan hardware SPI dan I2C
> *Bit banging is a NO NO*

## C
### Preparing WiringPi library for Raspberry Pi
Versi WiringPi pada raspberry pi sebelumnya adalah 2.50
Untuk menginstall versi terbaru, hapus WiringPi dengan
```sh
# Hapus wiringpi
$ sudo apt purge wiringpi
# Hapus semua yang mengingat lokasi wiringpi
$ hash -r
```

Reinstall versi terbaru dari fork di github:
```sh
$ git clone https://github.com/WiringPi/WiringPi.git
$ cd WiringPi
$ ./build
```
Cek versi terbaru
```sh
$ gpio -v
# Output
gpio version: 2.70
...
```
