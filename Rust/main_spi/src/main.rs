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

use reqwest;
use rppal::spi::{Bus, Mode, SlaveSelect, Spi};
use std::{error::Error, str, thread, time};

const MESSAGE_LENGTH: usize = 58;

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    // Configure SPI slave
    let spi = Spi::new(Bus::Spi0, SlaveSelect::Ss0, 400_000, Mode::Mode0)?;

    let mut read_buffer: [u8; MESSAGE_LENGTH] = [0u8; MESSAGE_LENGTH];
    let mut write_buffer: [u8; MESSAGE_LENGTH] = [0u8; MESSAGE_LENGTH];

    let client = reqwest::Client::new();
    let mut counter = 0;

    while counter < 1805 {
        write_buffer[0] = 0x10;

        let time_start = time::Instant::now();

        spi.transfer(&mut read_buffer, &mut write_buffer)?;

        let payload = str::from_utf8(&read_buffer).unwrap();

        let timestamp: u64 = time::SystemTime::now()
            .duration_since(time::UNIX_EPOCH)
            .unwrap()
            .as_secs();

        let mut data: String = timestamp.to_string();
        data = data + "|" + payload;

        let post_form = [("node", "node1"), ("data", &data)];

        println!("{data}");

        client
            .post("http://34.28.200.114/api")
            .form(&post_form)
            .send().await;
        
        let elapsed = time_start.elapsed();

        let total_wait = time::Duration::from_secs(2) - elapsed;
        counter += 1;

        thread::sleep(total_wait);
    }
    Ok(())
}
