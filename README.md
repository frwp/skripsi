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
Setiap bahasa pemrograman memerlukan persiapan tersendiri sebelum dapat digunakan. Terdapat bahasa pemrograman yang harus dipersiapkan lingkungan kerjanya (working environment) pada perangkat Raspberry Pi langsung seperti runtime untuk Java dan interpreter untuk Python dan Javascript. Meskipun C# merupakan bahasa yang memiliki konsep virtual machine seperti Java, C# tidak perlu memasang runtime pada perangkat Raspberry Pi karena C# mendukung self-contained application, yang artinya program yang dibuat sudah termasuk dengan runtime yang dibutuhkan untuk dapat dijalankan.
	
Bahasa compiled seperti Go dan Rust dapat dibangun di perangkat lain dengan melakukan cross-compilation, dalam hal ini proses compile dilakukan di perangkat laptop Asus X550IU sebagai perangkat pemrogram utama, sama seperti di mana program Arduino dibuat. Meskipun C juga merupakan bahasa yang bersifat compiled language, program C dapat dibuat di Raspberry Pi langsung karena lingkungan kerja untuk bahasa C sudah tersedia di sistem operasi Raspberry Pi OS, tidak seperti Go dan Rust.
	
    Untuk mempersiapkan lingkungan kerja bahasa Go, dapat dilakukan dengan langkah-langkah berikut:
    Menuju laman http://go.dev/dl dan memilih paket aplikasi yang sesuai, dalam hal ini sistem operasi dari Asus adalah linux Fedora 36 64-bit sehingga dipilih paket dengan OS Linux dan Arch x86-64.
    Ekstraksi paket ke folder /usr/local dengan perintah sudo tar -C /usr/local -xzf go1.19.5.linux-amd64.tar.gz (disesuaikan dengan paket yang telah diunduh) pada jendela terminal.
    Tambahkan lokasi instalasi ke PATH dengan perintah echo ‘export PATH=$PATH:/usr/local/go/bin’ >> $HOME/.profile  di terminal.

Periksa apakah Go sudah terpasang dengan benar dengan memasukkan perintah go version. Jika proses berjalan dengan benar, maka akan muncul versi Go yang terpasang.
Library yang digunakan untuk mengakses perangkat keras pada Raspberry Pi adalah Periph dari https://periph.io. 

	Untuk mempersiapkan lingkungan kerja bahasa Rust dilakukan dengan langkah-langkah berikut:
    Buka jendela terminal dan masukkan perintah berikut curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh .
    Pilih 1) Proceed with installation (default).
    Periksa apakah Rust sudah terpasang dengan perintah cargo version. Versi cargo yang terpasang akan ditampilkan apabila instalasi berjalan dengan benar. Cargo adalah package manager dan build tool untuk bahasa Rust.

Untuk menjalankan program dengan bahasa Java, diperlukan JRE (Java Runtime Environment) agar program dapat dijalankan di perangkat Raspberry Pi. Periksa apakah Java JRE sudah terinstall di Raspberry Pi dengan perintah java -version. Apabila muncul bash: java: command not found maka JRE belum terpasang. JRE dapat dipasang dengan perintah sudo apt install openjdk-11-jre.

    Pengembangan program dengan bahasa Java dilakukan di laptop Asus dengan melakukan beberapa persiapan seperti berikut:
    Pastikan dapat melakukan login ke Raspberry Pi dengan melalui SSH.
    Pasang Maven dan JDK (Java Development Kit) pada laptop Asus.
    Unduh raspimaven-archetype dari laman https://github.com/Pi4J/pi4j-maven-archetype.
    Unzip file yang yang telah diunduh ke suatu folder.
    Buka terminal dan masuk ke folder tempat unzip raspimaven-archetype dengan cd.
    Ketikkan perintah mvn install.

Pembuatan program dengan Java untuk Raspberry Pi sudah dapat dilakukan.

Untuk pengembangan program dengan bahasa C# dilakukan di laptop Asus. Untuk memulai pengembangan program dengan bahasa C#, .NET SDK perlu dipasang di laptop Asus dengan menjalankan perintah sudo dnf install dotnet-sdk-6.0 pada terminal.
Program yang dibuat dengan bahasa Javascript memerlukan runtime agar dapat berjalan di Raspberry Pi. Runtime yang digunakan adalah NodeJS. Versi NodeJS yang tersedia di repository Debian Buster adalah versi 10.24. Versi ini sudah terlalu lama dan sebaiknya digunakan versi LTS yaitu versi 18.xx.

    Untuk memasang NodeJS versi LTS dapat dilakukan dengan langkah berikut:
    Buka laman https://nodejs.org/en/download dan pilih Linux Binaries (ARM) versi ARMv7, klik kanan dan pilih copy link address.
    Buka terminal di Raspberry Pi dan masukkan perintah curl + link yang disalin.
    Ekstrak arsip yang diunduh dengan perintah tar -xJf node-v18.12.1-linux-armv7l.tar.xz.
    Pindahkan nodejs ke folder lib dengan perintah sudo mkdir -p /usr/lib/node && sudo mv node-v18.12.1-linux-armv7l/* /usr/lib/node/.
    Tambahkan nodejs ke PATH dengan perintah echo ‘export PATH=$PATH:/usr/lib/node/bin’ >> ~/.profile .
    Restart terminal dan periksa apakah NodeJS sudah terpasang dengan perintah node --version dan npm --version. Node adalah runtime untuk Javascript dan npm adalah package manager untuk kode Javascript.

Interpreter Python sudah terpasang di Raspberry Pi OS, akan tetapi versi yang digunakan adalah Python 3, sedangkan yang sudah pasti terpasang di Raspberry Pi OS adalah Python 2. Untuk memeriksa apakah Python 3 sudah terpasang dapat menggunakan perintah python3 di terminal. Apabila muncul error bash: python3: command not found, artinya Python 3 belum terpasang. Untuk memasang Python 3, dapat menggunakan perintah sudo apt install python3.
Setelah semua runtime dan environment terpasang, dilanjutkan dengan implementasi untuk masing-masing protokol.