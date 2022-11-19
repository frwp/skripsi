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

const MESSAGE_LENGTH = 5

func main() {
	host.Init()
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
		read := make([]byte, MESSAGE_LENGTH)

		if err := conn.Tx(nil, read); err != nil {
			log.Fatal(err)
		}

		elapsed := time.Since(start_time)
		fmt.Printf("%v\n", string(read[:]))
		time.Sleep(time.Second*2 - elapsed)
	}
}
