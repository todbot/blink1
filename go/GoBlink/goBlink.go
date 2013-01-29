//
// goBlink.go
//
// blink(1) USB interface library and demo
//
// See https://github.com/GaryBoone/GoBlink for more information
//
// (c) Gary Boone, 2012. 
//
// To build and run:
// $ go build goBlink.go && ./goBlink
//
// Build processing command; do not delete unless you change the build layout:
// +build ignore
//

package main

import (
	"."
	"time"
)

func main() {

	b := blink.NewBlink()

	b.Blink(10, 250*time.Millisecond)
	time.Sleep(1 * time.Second)

	b.SetRGB(10, 45, 233)
	time.Sleep(1 * time.Second)

	b.Random(10, 250*time.Millisecond)
}
