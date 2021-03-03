package main

import (
	"bytes"
	"fmt"
	"image"
	//"image/color"
	_ "image/jpeg"
	_ "image/png"
	"io/ioutil"
	"log"
	"math"
	"time"

	"./QRreader/goqr"
)

func recognizeFile(path string) {
	start := time.Now()
	//fmt.Printf("recognize file: %v\n", path)
	imgdata, err := ioutil.ReadFile(path)
	if err != nil {
		fmt.Printf("%v\n", err)
		return
	}

	img, _, err := image.Decode(bytes.NewReader(imgdata))
	if err != nil {
		fmt.Printf("image.Decode error: %v\n", err)
		return
	}
	qrCodes, err := goqr.Recognize(img)
	if err != nil {
		fmt.Printf("Recognize failed: %v\n", err)
		return
	}

	for _, qrCode := range qrCodes {

		fmt.Printf("qrCode text: %s\n", qrCode.Payload)

	}
	elapsed := time.Since(start)
	log.Printf("Time taken to find QRcode: %s", elapsed)
	//fmt.Printf("QR corner location:  %v \n", goqr.GV)

	//p1.x
	angleTest(goqr.Gx1, goqr.Gy1, goqr.Gx2, goqr.Gy2)
	//angleTest(goqr.GV[0],)
	//fmt.Printf("image.Decode error: %v\n", goqr.)
}

func main() {
	recognizeFile("QRgrid800.png")

	//drawing2()
}

func angleTest(x1, y1, x2, y2 float64) {
	ydiff := y2 - y1
	xdiff := x2 - x1
	radians := math.Atan2(ydiff, xdiff)
	//fmt.Printf("Ydiff: %v\n", ydiff)
	//fmt.Printf("Xdiff: %v\n", xdiff)
	//fmt.Printf("radians: %v\n", radians)

	angle := int(radians * 180 / math.Pi)

	fmt.Printf("QR Rotation: %v degrees\n", angle)

}
