package main

import (
	"fmt"
	"io"
	"log"
	"net/http"
	"net/url"
	"time"

	"periph.io/x/conn/v3/i2c"
	"periph.io/x/conn/v3/i2c/i2creg"
	"periph.io/x/conn/v3/physic"
	"periph.io/x/host/v3"
)

const MESSAGE_LENGTH = 32
const SLAVE_ADDRESS = 0x04

func main() {

	// initiate periph library
	host.Init()

	// open the i2c bus
	bus, err := i2creg.Open("")
	if err != nil {
		log.Fatal(err)
	}

	// defer closing bus when program exits
	defer bus.Close()

	// From periph.io: the actual speed is lower than stated here,
	// 600 kHz here => 375 kHz actual speed, which is closer to 400 kHz target
	bus.SetSpeed(physic.KiloHertz * 600)
	device := &i2c.Dev{Addr: SLAVE_ADDRESS, Bus: bus}

	// run program for 1 hour or else
	// forever loop
	watch := time.Now()
	for time.Since(watch) < time.Minute*15 {
		// start counting time
		start_time := time.Now()

		// allocate memory for buffer
		write := []byte{0x10}
		read := make([]byte, MESSAGE_LENGTH)

		// do i2c transaction
		if err := device.Tx(write, read); err != nil {
			log.Println(err)
			continue
		}

		// calculate elapsed time
		elapsed := time.Since(start_time)

		// add timestamp to data
		data := fmt.Sprintf("%v\n", string(read[:]))
		data = fmt.Sprint(time.Now().Unix()) + "|" + data
		fmt.Printf("%v\n", data)

		// envelope the data in a post form
		formData := url.Values{
			"data": {data},
			"node": {"node1"},
		}

		// run in goroutine to not block the loop
		go sendData("http://192.168.8.185/api", formData)

		time.Sleep(time.Second*2 - elapsed)
	}
}

// sendData takes a url and data and sends a post request to the url with the data
func sendData(url string, data url.Values) {
	// Create a new post request to the url with the data
	resp, err := http.PostForm(url, data)
	// If there was an error, panic
	if err != nil {
		log.Println(err)
	}

	// Close the response body when the function returns
	defer resp.Body.Close()

	// Read the response body
	body, err := io.ReadAll(resp.Body)
	// If there was an error, panic
	if err != nil {
		log.Println(err)
	}
	// Print the response body to the console
	log.Println(string(body))
}
