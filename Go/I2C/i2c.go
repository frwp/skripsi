package main

import (
	"fmt"
	"log"
	"time"

	"periph.io/x/conn/v3/i2c"
	"periph.io/x/conn/v3/i2c/i2creg"
	"periph.io/x/host/v3"
)

func main() {
	host.Init()
	bus, err := i2creg.Open("")
	if err != nil {
		log.Fatal(err)
	}

	defer bus.Close()

	device := &i2c.Dev{Addr: 4, Bus: bus}

	for {
		write := []byte{0x10}
		read := make([]byte, 5)
		if err := device.Tx(write, read); err != nil {
			log.Fatal(err)
		}
		fmt.Printf("%v\n", string(read[:]))
		time.Sleep(time.Second * 2)
	}
}
