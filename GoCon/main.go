
package main

import "fmt"
import "log"
import "go.bug.st/serial"
import "strings"
//import "strconv"
import "reflect"
//import "sync"
import "bufio"
import "os"


func main() {
	// Retrieve the port list
	ports, err := serial.GetPortsList()
	if err != nil {
		log.Fatal(err)
	}
	if len(ports) == 0 {
		log.Fatal("No serial ports found!")
	}

	// Print the list of detected ports
	for _, port := range ports {
		fmt.Printf("Found port: %v\n", port)
	}

	// Open the first serial port detected at bps N81
	mode := &serial.Mode{
		BaudRate: 9600,
		Parity:   serial.NoParity,
		DataBits: 8,
		StopBits: serial.OneStopBit,
	}
	port, err := serial.Open(ports[0], mode)
	if err != nil {
		log.Fatal(err)
	}
	fmt.Println(reflect.TypeOf(port))
	
	// Read and print the response
	go readData(port)

	scanner(port)
	
}

func scanner(port serial.Port) {
	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
	  s := scanner.Text()

	  if (string(s[0]) == "_") {
		n, err := port.Write([]byte(s))
		if err != nil {
			log.Fatal(err)
			fmt.Printf("this error")
		}
		fmt.Printf("Sent %v bytes\n", n)
	  }else if (string(s[0]) == "-") {
				n, err := port.Write([]byte("_,64,"+string(s[2])+",1,1,"+string(s[4])+string(s[5])))
				if err != nil {
					log.Fatal(err)
					fmt.Printf("this error")
				}
				fmt.Printf("Sent %v bytes\n", n)
			  
		}else if (string(s[0]) == "+") {
			n, err := port.Write([]byte("_,64,"+string(s[2])+","+string(s[4])+",1000,0"))
			if err != nil {
				log.Fatal(err)
				fmt.Printf("this error")
			}
			fmt.Printf("Sent %v bytes\n", n)
	  
		}else {
			fmt.Printf("Enter right Format")
	  	}

	//readData(port)

	}
  }


func readData(port serial.Port)  {
	buff := make([]byte, 100)
	for {
		// Reads up to 100 bytes
		n, err := port.Read(buff)
		if err != nil {
			log.Fatal(err)
		}
		if n == 0 {
		//	fmt.Println("\nEOF")
		//	break
		}

		fmt.Printf("GOTBack: %s \n", string(buff[:n]))

		// If we receive a newline stop reading
		if strings.Contains(string(buff[:n]), "0.00") {
			break
		}
	}
}


