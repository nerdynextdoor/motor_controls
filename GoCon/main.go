
package main

import "fmt"
import "log"
import "go.bug.st/serial"
import "strings"
import "strconv"
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
	//moveSteps(10000,port)
	
	// Read and print the response
	go readData(port)

	scanner(port)
	
}

func scanner(port serial.Port) {
	scanner := bufio.NewScanner(os.Stdin)
	for scanner.Scan() {
	  //fmt.Println(scanner.Text())
	  s := strings.Split(scanner.Text(),",")
	  n, _ := strconv.Atoi(s[1])
	//  ch, _ :=  scanner.Text()[0]
	  //moveSteps(n,port)
	    send(s[0], n, port)
	// if (c == "m")
	//	moveSteps(n,port)
	// 
	// else if (c == "u")
	//	upDown(n,port)
	// 
	// else if (c == "s")
	//	shiftRL(n,port)
	// 
	// else if (c == "r")
	//	rotateRL(n,port)
	 

	}
  }
  func send(mode string, n int, port serial.Port)  {
	 if (mode == "m"){
		moveSteps(n,port)
	 }else if (mode == "u"){
		upDown(n,port)
	 }else if (mode == "s"){
		shiftRL(n,port)
	 }else if (mode == "r"){
		rotateRL(n,port)
	 }
		
  }

func moveSteps(dir int, port serial.Port){
	n, err := port.Write([]byte(strconv.Itoa(dir)+","+strconv.Itoa(dir)+","+strconv.Itoa(dir)+","+strconv.Itoa(dir)+","+strconv.Itoa(0)))
	if err != nil {
		log.Fatal(err)
	}
	fmt.Printf("Sent %v bytes\n", n)
}

func upDown(dir int, port serial.Port){
		n, err := port.Write([]byte(strconv.Itoa(0)+","+strconv.Itoa(0)+","+strconv.Itoa(0)+","+strconv.Itoa(0)+","+strconv.Itoa(dir)))
		if err != nil {
			log.Fatal(err)
		}
		fmt.Printf("Sent %v bytes\n", n)
	}

func shiftRL(dir int, port serial.Port){
		n, err := port.Write([]byte(strconv.Itoa((-1*dir))+","+strconv.Itoa(dir)+","+strconv.Itoa(dir)+","+strconv.Itoa((-1*dir))+","+strconv.Itoa(0)))
		if err != nil {
			log.Fatal(err)
		}
		fmt.Printf("Sent %v bytes\n", n)
	}
func rotateRL(dir int, port serial.Port){
		n, err := port.Write([]byte(strconv.Itoa((-1*dir))+","+strconv.Itoa(dir)+","+strconv.Itoa(-1*dir)+","+strconv.Itoa((dir))+","+strconv.Itoa(0)))
		if err != nil {
			log.Fatal(err)
		}
		fmt.Printf("Sent %v bytes\n", n)
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
			fmt.Println("\nEOF")
			break
		}

		fmt.Printf("%s", string(buff[:n]))

		// If we receive a newline stop reading
		if strings.Contains(string(buff[:n]), "0.00") {
			break
		}
	}
}


