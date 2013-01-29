//
// hid.go
// 
// Go interface for C HID USB driver (usblib)
// 
// See https://github.com/GaryBoone/GoBlink for more information
//
// (c) Gary Boone, 2012. 
//
// NOTE: For CGO, there should be no comments before the // #include lines.
// Also, there should be no space between // #include lines and import line!

package blink

// #include "hiddata.h"
// #include <stdlib.h>
// #cgo CFLAGS: -I/opt/local/include
// #cgo LDFLAGS: -L/opt/local/lib -lusb
import "C"

// import "unsafe"

import (
	"fmt"
	"log"
	"math/rand"
	"runtime"
	"time"
)

// taken from blink1/hardware/firmware/usbconfig.h
var (
	IDENT_VENDOR_NUM     C.int
	IDENT_PRODUCT_NUM    C.int
	IDENT_VENDOR_STRING  *C.char
	IDENT_PRODUCT_STRING *C.char
)

const fadeMs = 100

func init() {
	IDENT_VENDOR_NUM = C.int(0x27B8)
	IDENT_PRODUCT_NUM = C.int(0x01ED)
	IDENT_VENDOR_STRING = C.CString("ThingM") // C strings need to be freed
	IDENT_PRODUCT_STRING = C.CString("blink(1)")

	// Those C strings could be freed using
	// C.free(unsafe.Pointer(IDENT_VENDOR_STRING))
	// C.free(unsafe.Pointer(IDENT_PRODUCT_STRING))
}

type Blink struct {
	dev *C.struct_usbDevice_t
}

func NewBlink() *Blink {
	blink := Blink{}
	blink.open()
	return &blink
}

// blink white num times: on for blinkMs milliseconds then off for blinkMs milliseconds 
func (b *Blink) Blink(num int, blinkMs time.Duration) {
	blink(b.dev, num, blinkMs)
}

func (bl *Blink) SetRGB(r, g, b int) {
	setRGB(bl.dev, r, g, b)
}

// show num random colors, each for blinkMs milliseconds
func (b *Blink) Random(num int, blinkMs time.Duration) {
	random(b.dev, num, blinkMs)
}

func (b *Blink) open() {
	if !openBlink1(&b.dev) {
		log.Print("error: couldn't open blink1.")
		return
	}
	runtime.SetFinalizer(b, (*Blink).close)
}

func (b *Blink) close() {
	C.usbhidCloseDevice(b.dev)
}

func blink(dev *C.struct_usbDevice_t, num int, blinkMs time.Duration) {
	v := [2]int{0, 255}
	for i := 0; i < num*2; i++ {
		rc := fadeToRgbBlink1(dev, fadeMs, v[i%2], v[i%2], v[i%2])
		if rc != 0 { // on error, do something, anything. come on.
			log.Print("error in blink: couldn't open blink1. Error: ", errorMsgBlink1(rc))
		}
		// log.Printf("%d: %d, %x,%x,%x \n", i, millis, v[i%2], v[i%2], v[i%2])
		time.Sleep(blinkMs)
	}
}

func random(dev *C.struct_usbDevice_t, num int, blinkMs time.Duration) {
	for i := 0; i < num; i++ {
		r := rand.Intn(255)
		g := rand.Intn(255)
		b := rand.Intn(255)
		rc := setRGB(dev, r, g, b)
		if rc != 0 { // on error, do something, anything. come on.
			log.Print("error in blink: couldn't open blink1. Error: ", errorMsgBlink1(rc))
		}
		// log.Printf("%d: %d, %x,%x,%x \n", i, millis, v[i%2], v[i%2], v[i%2])
		time.Sleep(blinkMs)
	}
}

// Open up a blink(1) for transactions.
// returns 0 on success, and opened device in "dev"
// or returns non-zero error that can be decoded with blink1_error_msg()
// FIXME: what happens when multiple are plugged in? 
func openBlink1(dev **C.struct_usbDevice_t) bool {

	rc := C.usbhidOpenDevice(dev,
		IDENT_VENDOR_NUM, IDENT_VENDOR_STRING,
		IDENT_PRODUCT_NUM, IDENT_PRODUCT_STRING,
		1) // NOTE: '0' means "not using report IDs"

	if rc != 0 {
		log.Print("error in open: couldn't open blink1. Error: ", errorMsgBlink1(rc))
		return false
	}

	return true
}

// 
// Close a Blink1 
// 
func closeBlink1(dev *C.usbDevice_t) {
	C.usbhidCloseDevice(dev)
}

//
// Decode the error messages
// 
func errorMsgBlink1(errCode C.int) string {

	switch errCode {
	case C.USBOPEN_ERR_ACCESS:
		return "Access to device denied"
	case C.USBOPEN_ERR_NOTFOUND:
		return "The specified device was not found"
	case C.USBOPEN_ERR_IO:
		return "Communication error with device"
	default:
		return fmt.Sprintf("Unknown USB error %d", errCode)
	}
	return "" /* not reached */
}

func setRGB(dev *C.usbDevice_t, r, g, b int) C.int {
	return fadeToRgbBlink1(dev, fadeMs, r, g, b)
}

func fadeToRgbBlink1(dev *C.usbDevice_t, fadeMillis, r, g, b int) C.int {
	var buf [9]C.char

	if dev == nil {
		return -1 // BLINK1_ERR_NOTOPEN;
	}

	dms := C.char(fadeMillis / 10) // millis_divided_by_10

	buf[0] = C.char(0)
	buf[1] = C.char('c')
	buf[2] = C.char(r)
	buf[3] = C.char(g)
	buf[4] = C.char(b)
	buf[5] = C.char((dms >> 8))
	buf[6] = C.char(dms % 127)
	buf[7] = 0

	err := C.usbhidSetReport(dev, &buf[0], C.int(8))
	if err != 0 {
		log.Printf("error writing data: %s\n", errorMsgBlink1(err))
	}
	return err // FIXME: remove fprintf
}
