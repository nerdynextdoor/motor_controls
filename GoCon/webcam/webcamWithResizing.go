// Example program that uses blakjack/webcam library
// for working with V4L2 devices.
package main

import (
	"bytes"
	"flag"
	"fmt"
	"image"
	//"image/color"
	"image/draw"
	"image/jpeg"
	_ "image/png"
	//"imageutil"
	"log"
	"mime/multipart"
	"net/http"
	"net/textproto"
	"os"
	"sort"
	"strconv"
	"time"
	//"io/ioutil"
	"math"

	"./QRreader/goqr"
	"github.com/blackjack/webcam"
	//"github.com/nerdynextdoor/Go_QR_Grid/imageutil"
	"github.com/nfnt/resize"
)

const (
	V4L2_PIX_FMT_PJPG = 0x47504A50
	V4L2_PIX_FMT_YUYV = 0x56595559
)

type FrameSizes []webcam.FrameSize

var CP = true

func (slice FrameSizes) Len() int {
	return len(slice)
}

//For sorting purposes
func (slice FrameSizes) Less(i, j int) bool {
	ls := slice[i].MaxWidth * slice[i].MaxHeight
	rs := slice[j].MaxWidth * slice[j].MaxHeight
	return ls < rs
}

//For sorting purposes
func (slice FrameSizes) Swap(i, j int) {
	slice[i], slice[j] = slice[j], slice[i]
}

var supportedFormats = map[webcam.PixelFormat]bool{
	V4L2_PIX_FMT_PJPG: true,
	V4L2_PIX_FMT_YUYV: true,
}

func readChoice(s string) int {
	var i int
	for true {
		print(s)
		_, err := fmt.Scanf("%d\n", &i)
		if err != nil || i < 1 {
			println("Invalid input. Try again")
		} else {
			break
		}
	}
	return i
}

func main() {
	dev := flag.String("d", "/dev/video0", "video device to use")
	//fmtstr := flag.String("f", "", "video format to use, default first supported")
	//szstr := flag.String("s", "640x480", "frame size to use, default largest one")
	single := flag.Bool("m", false, "single image http mode, default mjpeg video")
	addr := flag.String("l", ":8080", "addr to listien")
	fps := flag.Bool("p", true, "print fps info")
	flag.Parse()

	cam, err := webcam.Open(*dev)
	if err != nil {
		panic(err.Error())
	}
	defer cam.Close()

	// select pixel format

	format_desc := cam.GetSupportedFormats()
	var formats []webcam.PixelFormat
	for f := range format_desc {
		formats = append(formats, f)
	}

	println("Available formats: ")
	for i, value := range formats {
		fmt.Fprintf(os.Stderr, "[%d] %s\n", i+1, format_desc[value])
	}

	choice := readChoice(fmt.Sprintf("Choose format [1-%d]: ", len(formats)))
	format := formats[choice-1]

	// select frame size
	fmt.Fprintf(os.Stderr, "Supported frame sizes for format %s\n", format_desc[format])
	frames := FrameSizes(cam.GetSupportedFrameSizes(format))
	sort.Sort(frames)

	for i, value := range frames {
		fmt.Fprintf(os.Stderr, "[%d] %s\n", i+1, value.GetString())
	}
	choice = readChoice(fmt.Sprintf("Choose format [1-%d]: ", len(frames)))
	size := frames[choice-1]

	f, w, h, err := cam.SetImageFormat(format, uint32(size.MaxWidth), uint32(size.MaxHeight))

	if err != nil {
		panic(err.Error())
	} else {
		fmt.Fprintf(os.Stderr, "Resulting image format: %s (%dx%d)\n", format_desc[f], w, h)
	}

	println("Press Enter to start streaming")
	fmt.Scanf("\n")

	// start streaming
	err = cam.StartStreaming()
	if err != nil {
		log.Println(err)
		return
	}

	var (
		li   chan *bytes.Buffer = make(chan *bytes.Buffer)
		fi   chan []byte        = make(chan []byte)
		back chan struct{}      = make(chan struct{})
	)
	go encodeToImage(cam, back, fi, li, w, h, f)
	if *single {
		go httpImage(*addr, li)
	} else {
		go httpVideo(*addr, li)
	}

	timeout := uint32(5) //5 seconds
	start := time.Now()
	var fr time.Duration

	for {
		err = cam.WaitForFrame(timeout)
		if err != nil {
			log.Println(err)
			return
		}

		switch err.(type) {
		case nil:
		case *webcam.Timeout:
			log.Println(err)
			continue
		default:
			log.Println(err)
			return
		}

		frame, err := cam.ReadFrame()
		if err != nil {
			log.Println(err)
			return
		}
		if len(frame) != 0 {

			// print framerate info every 10 seconds
			fr++
			if *fps {
				if d := time.Since(start); d > time.Second*10 {
					fmt.Println(float64(fr)/(float64(d)/float64(time.Second)), "fps")
					start = time.Now()
					fr = 0
				}
			}

			select {
			case fi <- frame:
				<-back
			default:
			}
		}
	}
}

func encodeToImage(wc *webcam.Webcam, back chan struct{}, fi chan []byte, li chan *bytes.Buffer, w, h uint32, format webcam.PixelFormat) {
	imgFile1, err := os.Open("./Tracker.png")
	if err != nil {
		fmt.Println(err)
	}

	tracker, _, err := image.Decode(imgFile1)
	if err != nil {
		fmt.Println(err)
	}
	var (
		frame []byte
		img   image.Image
		temp  image.Image
		td    image.Image
	)
	for {
		bframe := <-fi
		// copy frame
		if len(frame) < len(bframe) {
			frame = make([]byte, len(bframe))
		}
		copy(frame, bframe)
		back <- struct{}{}

		switch format {
		case V4L2_PIX_FMT_YUYV:
			//log.Println(format)
			yuyv := image.NewYCbCr(image.Rect(0, 0, int(w), int(h)), image.YCbCrSubsampleRatio422)
			for i := range yuyv.Cb {
				ii := i * 4
				yuyv.Y[i*2] = frame[ii]
				yuyv.Y[i*2+1] = frame[ii+2]
				yuyv.Cb[i] = frame[ii+1]
				yuyv.Cr[i] = frame[ii+3]
			}
			img = yuyv

			img = toRgb(img)
			//	p := color.RGBA(yuyv)
			//	fmt.Printf("newVar = %T\n", p)
			m := resize.Resize(300, 0, tracker, resize.Lanczos3)
			//DrawYCbCr.
			td = m
			//fmt.Printf("var1 = %T\n", td)
			//b := src.Bounds()
			//m = image.NewRGBA(image.YCbCrSubsampleRatio422)
			//m = color.RGBA(image.YCbCrSubsampleRatio422)
			//	l.DrawYCbCr(p, tracker.Bounds(), tracker, image.Point{0, 0})
			draw.Draw(img, tracker.Bounds(), img, image.Point{0, 0}, draw.Src)
			fmt.Printf("var1 = %T\n", td)
			//img = td
			fmt.Printf("var2 = %T\n", img)
			if temp != img {
				if CP == true {
					go recognizeFile(img)
				}
			}
			temp = img
		default:
			log.Println(format)
			log.Fatal("invalid format ?")
		}
		//convert to jpeg
		buf := &bytes.Buffer{}
		if err := jpeg.Encode(buf, img, nil); err != nil {
			log.Fatal(err)
			return
		}
		fmt.Printf("newVar = %T\n", buf)
		const N = 50
		// broadcast image up to N ready clients
		nn := 0
	FOR:
		for ; nn < N; nn++ {
			select {
			case li <- buf:
			default:
				break FOR
			}
		}
		if nn == 0 {
			li <- buf
		}

	}
}
func toRgb(src image.Image) *image.RGBA {
	b := src.Bounds()
	dst := image.NewRGBA(image.Rect(0, 0, b.Dx(), b.Dy()))
	draw.Draw(dst, b, src, image.ZP, draw.Src)
	return dst
}

// func convertFiles(w, h int, filter Filter, rgb bool) (image.Image, image.Image) {
// 	//src := readImage(t, input)
// 	raw := image.NewYCbCr(image.Rect(0, 0, w*2, h*2), image.YCbCrSubsampleRatio420)
// 	dst := raw.SubImage(image.Rect(7, 7, 7+w, 7+h))
// 	if rgb {
// 		src = toRgb(src)
// 		dst = toRgb(dst)
// 	}
// 	err := Convert(dst, src, filter)
// 	expect(t, err, nil)
// 	return src, dst
// }
// func Thumbnail(dst draw.Image, src image.Image) error {
// 	// Scale down src in the dimension that is closer to dst.
// 	sb := src.Bounds()
// 	db := dst.Bounds()
// 	rx := float64(sb.Dx()) / float64(db.Dx())
// 	ry := float64(sb.Dy()) / float64(db.Dy())
// 	var b image.Rectangle
// 	if rx < ry {
// 		b = image.Rect(0, 0, db.Dx(), int(float64(sb.Dy())/rx))
// 	} else {
// 		b = image.Rect(0, 0, int(float64(sb.Dx())/ry), db.Dy())
// 	}

// 	buf := image.NewRGBA(b)
// 	if err := Scale(buf, src); err != nil {
// 		return err
// 	}

// 	// Crop.
// 	// TODO(crawshaw): improve on center-alignment.
// 	var pt image.Point
// 	if rx < ry {
// 		pt.Y = (b.Dy() - db.Dy()) / 2
// 	} else {
// 		pt.X = (b.Dx() - db.Dx()) / 2
// 	}
// 	draw.Draw(dst, db, buf, pt, draw.Src)
// 	return nil
// }

func httpImage(addr string, li chan *bytes.Buffer) {
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		log.Println("connect from", r.RemoteAddr, r.URL)
		if r.URL.Path != "/" {
			http.NotFound(w, r)
			return
		}

		//remove stale image
		<-li

		img := <-li

		w.Header().Set("Content-Type", "image/jpeg")

		if _, err := w.Write(img.Bytes()); err != nil {
			log.Println(err)
			return
		}

	})

	log.Fatal(http.ListenAndServe(addr, nil))
}

func httpVideo(addr string, li chan *bytes.Buffer) {
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		log.Println("connect from", r.RemoteAddr, r.URL)
		if r.URL.Path != "/" {
			http.NotFound(w, r)
			return
		}

		//remove stale image
		<-li
		const boundary = `frame`
		w.Header().Set("Content-Type", `multipart/x-mixed-replace;boundary=`+boundary)
		multipartWriter := multipart.NewWriter(w)
		multipartWriter.SetBoundary(boundary)
		for {
			img := <-li
			image := img.Bytes()
			iw, err := multipartWriter.CreatePart(textproto.MIMEHeader{
				"Content-type":   []string{"image/jpeg"},
				"Content-length": []string{strconv.Itoa(len(image))},
			})
			if err != nil {
				log.Println(err)
				return
			}
			_, err = iw.Write(image)
			if err != nil {
				log.Println(err)
				return
			}
		}
	})

	log.Fatal(http.ListenAndServe(addr, nil))
}

func recognizeFile(img image.Image) {
	start := time.Now()
	CP = false

	qrCodes, err := goqr.Recognize(img)
	if err != nil {
		fmt.Printf("Recognize failed: %v\n", err)
		elapsed := time.Since(start)
		log.Printf("Time taken to find QRcode: %s \n", elapsed)
		CP = true
		return
	}

	for _, qrCode := range qrCodes {

		fmt.Printf("qrCode text: %s\n", qrCode.Payload)

	}
	elapsed := time.Since(start)
	log.Printf("Time taken to find QRcode: %s", elapsed)
	fmt.Printf("QR corner location:  %v \n", goqr.GV)

	angleTest(goqr.Gx1, goqr.Gy1, goqr.Gx2, goqr.Gy2)
	CP = true
}

func angleTest(x1, y1, x2, y2 float64) {
	ydiff := y2 - y1
	xdiff := x2 - x1
	radians := math.Atan2(ydiff, xdiff)

	angle := int(radians * 180 / math.Pi)

	fmt.Printf("QR Rotation: %v degrees\n", angle)

}
