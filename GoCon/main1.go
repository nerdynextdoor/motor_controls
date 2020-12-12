// Blink
//
// Blinks an LED for one second on, one second off.
// This is heavily annotated, with error handling.

package main

import "fmt"
import "log"
import "github.com/jacobsa/go-serial/serial"
import "bufio"

func main() {

	// Set up options.
	options := serial.OpenOptions{
		PortName:        "/dev/ttyS0",
		BaudRate:        115200,
		DataBits:        8,
		StopBits:        1,
		MinimumReadSize: 4,
	}

	serialPort, err := serial.Open(options)
	if err != nil {
		log.Fatalf("serial.Open: %v", err)
	}
	fmt.Println(1)
	defer serialPort.Close()
	reader := bufio.NewReader(serialPort)
	scanner := bufio.NewScanner(reader)
	fmt.Println(2)
	for scanner.Scan() {
		fmt.Println(scanner.Text())
		fmt.Println(3)
	}
	fmt.Println(scanner.Text())
}
