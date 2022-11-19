package main

import (
	"fmt"
	"log"
	"time"

	"periph.io/x/conn/v3/i2c"
	"periph.io/x/conn/v3/i2c/i2creg"
	"periph.io/x/conn/v3/physic"
	"periph.io/x/host/v3"
)

const MESSAGE_LENGTH = 5
const SLAVE_ADDRESS = 0x04

func main() {
	host.Init()
	bus, err := i2creg.Open("")
	if err != nil {
		log.Fatal(err)
	}

	defer bus.Close()

	// From periph.io: the actual speed is lower than stated here,
	// 600 kHz here => 375 kHz actual speed, which is closer to 400 kHz target
	bus.SetSpeed(physic.KiloHertz * 600)
	device := &i2c.Dev{Addr: SLAVE_ADDRESS, Bus: bus}

	for {
		start_time := time.Now()
		read := make([]byte, MESSAGE_LENGTH)

		if err := device.Tx(nil, read); err != nil {
			log.Fatal(err)
		}

		elapsed := time.Since(start_time)
		fmt.Printf("%v\n", string(read[:]))
		time.Sleep(time.Second*2 - elapsed)
	}
}
