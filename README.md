# motor_controls
All golang code is in GOCon

# 1st
follow the steps here to free your serial pin https://www.abelectronics.co.uk/kb/article/1035/raspberry-pi-3--4-and-zero-w-serial-port-usage

# 2nd
To run main.go in GoCon, you need to import the library by running 
    
    import "go.bug.st/serial"
    
# 3rd 
run this in the GoCon dir

    go run main.go

if you run into cannot find package “golang.org/x/sys/unix””error message
  
  FIX: go get golang.org/x/sys/unix

# 4th 
if the program runs without any error connect the RX pin of the pi to the TX pin of the Arduino

# 5th 
run code "_,64,m,1,3400"

