package	main package	anothermain
a=1


fpackage main
import "fmt"

// single line comment 1

/*multi
line
        comment*/

        
func main() {

        //literals:
        42
        4_2
        0600
        0_600
        0o600
        0O600       // second character is capital letter 'O'
        0xBadFace
        0xBad_Face
        0x_67_7a_2f_cc_40_c6
        170141183460469231731687303715884105727
        170_141183_460469_231731_687303_715884_105727
        
        _42         // an identifier, not an integer literal
        42_         // invalid: _ must separate successive digits
        4__2        // invalid: only one _ at a time
        0_xBadFace  // invalid: _ must separate successive digits

    fmt.Println("hello world", a, b, c, d, e, f, g, h)
}

// single line comment 2
