package main

import (
	"fmt"
)

func main() {
    // INTS:
	42          //42 is integer_lit
    4_2         //4_2 is integer_lit
    0600        //0600 is integer_lit
    0_600       //0_600 is integer_lit
    0o600       //0o600 is integer_lit
    0O600       //0O600 is integer_lit
    0xBadFace   //0xBadFace is integer_lit
    0xBad_Face  //0xBad_Face is integer_lit
    0x_67_7a_2f_cc_40_c6    //0x_67_7a_2f_cc_40_c6 is integer_lit
    170141183460469231731687303715884105727 //170141183460469231731687303715884105727 is integer_lit
    170_141183_460469_231731_687303_715884_105727   //170_141183_460469_231731_687303_715884_105727 is integer_lit

    _42         // _42 is NOT integer_lit
    42_         // 42_ is NOT integer_lit
    4__2        // 4__2 is NOT integer_lit
    0_xBadFace  // 0_xBadFace is NOT integer_lit

    // FLOATS:
    0.
    72.40
    072.40       // == 72.40
    2.71828
    1.e+0
    6.67428e-11
    1E6
    .25
    .12345E+5
    1_5.         // == 15.0
    0.15e+0_2    // == 15.0
    
    0x1p-2       // == 0.25
    0x2.p10      // == 2048.0
    0x1.Fp+0     // == 1.9375
    0X.8p-0      // == 0.5
    0X_1FFFP-16  // == 0.1249847412109375
    0x15e-2      // == 0x15e - 2 (integer subtraction)
    
    0x.p1        // invalid: mantissa has no digits
    1p-2         // invalid: p exponent requires hexadecimal mantissa
    0x1.5e-2     // invalid: hexadecimal mantissa requires p exponent
    1_.5         // invalid: _ must separate successive digits
    1._5         // invalid: _ must separate successive digits
    1.5_e1       // invalid: _ must separate successive digits
    1.5e_1       // invalid: _ must separate successive digits
    1.5e1_       // invalid: _ must separate successive digits

    // IMAGINARIES:
    0i
    0123i         // == 123i for backward-compatibility
    0o123i        // == 0o123 * 1i == 83i
    0xabci        // == 0xabc * 1i == 2748i
    0.i
    2.71828i
    1.e+0i
    6.67428e-11i
    1E6i
    .25i
    .12345E+5i
    0x1p-2i       // == 0x1p-2 * 1i == 0.25i
    fmt.Println("Hello, playground", a)
}
