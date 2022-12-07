/*******************************************************************************
 * SPI master code @ Raspberry Pi in Rust language
 * @author Fransiskus Rian Wardana Putra
 * 7 Des 2022
 *
 * Kode ini digunakan untuk tugas akhir S1 Ilmu Komputer dengan topik
 * Bahasa pemrograman dan protokol komunikasi untuk IoT
 *
 * Kode ini menggunakan library RPPAL untuk melakukan setup ke hardware SPI
 * pada perangkat Raspberry Pi. Lihat github.com/golemparts/rppal
 *
 * To build this file, run the script:
 * > ./build.sh
 *******************************************************************************/

use std::{error::Error, str, thread, time};

use rppal::spi::{Bus, Mode, SlaveSelect, Spi};

const MESSAGE_LENGTH: usize = 58;

fn main() -> Result<(), Box<dyn Error>> {
    // Configure SPI slave
    let spi = Spi::new(Bus::Spi0, SlaveSelect::Ss0, 400_000, Mode::Mode0)?;

    let mut read_buffer: [u8; MESSAGE_LENGTH] = [0u8; MESSAGE_LENGTH];
    let mut write_buffer: [u8; MESSAGE_LENGTH] = [0u8; MESSAGE_LENGTH];

    loop {
        write_buffer[0] = 0x10;

        let time_start = time::Instant::now();

        spi.transfer(&mut read_buffer, &mut write_buffer)?;

        let elapsed = time_start.elapsed();

        let payload = str::from_utf8(&read_buffer).unwrap();
        println!("{payload}");

        let total_wait = time::Duration::from_secs(2) - elapsed;

        thread::sleep(total_wait);
    }
    // Ok(())
}
