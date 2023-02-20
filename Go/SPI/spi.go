package main

import (
	"fmt"
	"io"
	"log"
	"net/http"
	"net/url"
	"time"

	"periph.io/x/conn/v3/physic"
	"periph.io/x/conn/v3/spi"
	"periph.io/x/conn/v3/spi/spireg"
	"periph.io/x/host/v3"
)

const MESSAGE_LENGTH = 58

func main() {

	// initiate periph library
	host.Init()

	// open the next available "port" (usually /dev/spidev0.0 or /dev/spidev0.1)
	port, err := spireg.Open("")
	if err != nil {
		log.Fatal(err)
	}

	// defer closing port when program exits
	defer port.Close()

	// From periph.io: the actual speed is lower than stated here,
	// 600 kHz here => 375 kHz actual speed, which is closer to 400 kHz target
	conn, err := port.Connect(physic.KiloHertz*600, spi.Mode0, 8)
	if err != nil {
		log.Fatal(err)
	}

	counter := 0

	// forever loop
	for counter < 1905 {
		// start counting time
		start_time := time.Now()

		// alocate memory for buffer
		write := make([]byte, MESSAGE_LENGTH)
		read := make([]byte, MESSAGE_LENGTH)

		// mark begin byte
		write[0] = 0x10

		// do spi transaction, full duplex
		if err := conn.Tx(write, read); err != nil {
			log.Fatal(err)
		}

		// calculate elapsed time
		elapsed := time.Since(start_time)

		// add timestamp to data
		data := fmt.Sprintf("%v\n", string(read[:]))
		data = fmt.Sprint(time.Now().Unix()) + "|" + data
		fmt.Printf("%v\n", data)

		// envelope the data in post form
		formData := url.Values{
			"data": {data},
			"node": {"node1"},
		}

		// run in goroutine to not block the loop
		go sendData("http://34.28.200.114/api", formData)
		counter += 1

		time.Sleep(time.Second*2 - elapsed)
	}
}

// sendData takes a url and data and sends a post request to the url with the data
func sendData(url string, data url.Values) {
	// Create a new post request to the url with the data
	resp, err := http.PostForm(url, data)
	// If there was an error, panic
	if err != nil {
		log.Panic(err)
	}

	// Close the response body when the function returns
	defer resp.Body.Close()

	// Read the response body
	body, err := io.ReadAll(resp.Body)
	// If there was an error, panic
	if err != nil {
		log.Panic(err)
	}
	// Print the response body to the console
	log.Println(string(body))
}
