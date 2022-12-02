package main

import (
	"fmt"
	"log"
	"time"

	"periph.io/x/conn/v3/physic"
	"periph.io/x/conn/v3/spi"
	"periph.io/x/conn/v3/spi/spireg"
	"periph.io/x/host/v3"
)

const MESSAGE_LENGTH = 42

func main() {
	host.Init()

	// open the next available "port" (usually /dev/spidev0.0 or /dev/spidev0.1)
	port, err := spireg.Open("")
	if err != nil {
		log.Fatal(err)
	}

	defer port.Close()

	// From periph.io: the actual speed is lower than stated here,
	// 600 kHz here => 375 kHz actual speed, which is closer to 400 kHz target
	conn, err := port.Connect(physic.KiloHertz*600, spi.Mode0, 8)
	if err != nil {
		log.Fatal(err)
	}

	for {
		start_time := time.Now()
		write := make([]byte, MESSAGE_LENGTH)
		write[0] = 0x10
		read := make([]byte, MESSAGE_LENGTH)

		if err := conn.Tx(write, read); err != nil {
			log.Fatal(err)
		}

		elapsed := time.Since(start_time)
		fmt.Printf("%v\n", string(read[:]))
		time.Sleep(time.Second*2 - elapsed)
	}
}
