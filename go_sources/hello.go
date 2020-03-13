package main
fpackage main
import "fmt"

// single line comment 1

/*multi
line
        comment*/

        
func main() {

        //literals:
a := 42
b := 4_2
c := 0600
d := 0_600
e := 0o600
f := 0O600      
g := 0xBadFace
h := 0xBad_Face
    fmt.Println("hello world", a, b, c, d, e, f, g, h)
}

// single line comment 2
