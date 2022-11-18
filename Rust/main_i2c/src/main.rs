/*******************************************************************************
 * I2C master code @ Raspberry Pi in Rust language
 * @author Fransiskus Rian Wardana Putra
 * 18 Nov 2022
 *
 * Kode ini digunakan untuk tugas akhir S1 Ilmu Komputer dengan topik
 * Bahasa pemrograman dan protokol komunikasi untuk IoT
 *
 * Kode ini menggunakan library RPPAL untuk melakukan setup ke hardware I2C
 * pada perangkat Raspberry Pi. Lihat github.com/golemparts/rppal
 *
 * Perangkat dihubungkan sebagai berikut:
 * (Raspberry Pi)   (Arduino Nano)
 * GND          ->  GND
 * 5V           ->  5V (VCC)
 * SDA          ->  A4 (SDA) (dengan pull-up resistor 4.7K ke VCC)
 * SCL          ->  A5 (SCL) (dengan pull-up resistor 4.7K ke VCC)
 *
 * To build this file, run:
 * > cargo build
 *******************************************************************************/

 use std::error::Error;
 use std::str;
 
 use rppal::i2c::I2c;
 
 const SLAVE_ADDRESS: u16 = 0x04;
 const MESSAGE_LENGTH: usize = 5;
 
 fn main() -> Result<(), Box<dyn Error>>{
     let mut i2c = I2c::new()?;
     let mut buffer: [u8; 5] = [0u8; MESSAGE_LENGTH];
     
     // Set the address of the slave
     i2c.set_slave_address(SLAVE_ADDRESS)?;
 
     i2c.read(&mut buffer)?;
     let payload = str::from_utf8(&buffer).unwrap();
     println!("{payload}");
     Ok(())
 }
 