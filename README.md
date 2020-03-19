# Golang lexer
#### Compiler Design - Project #1 - Faculty of Mathematics and Computer Science, University of Bucharest

## Implementation: 

- After the official golang specification: https://golang.org/ref/spec
- CPP using only STL and with all the code in one file

## Tokens progress:

- [x] Single line comment
- [x] Multi line comments
- [x] Identifiers
- [x] Keywords
- [x] Operators and punctuation
- [x] Integer literals
- [x] Floating-point literals
- [x] Imaginary literals
- [ ] Rune literals - **stuck**:
```
ECMAScript, as well as all other flavors supported by the std::regex, does not support Unicode properties (Unicode category classes).
```
- [ ] String literals - **stuck**:
```
ECMAScript, as well as all other flavors supported by the std::regex, does not support Unicode properties (Unicode category classes).
```
## Tokens and regex execution order:

### EBNF = Extended Backus–Naur form

- single line comments and multi line comments (skip)
- keywords
    constant strings (eg: "if")
- identifiers       
    ```                
    EBNF identifier = letter { letter | unicode_digit } .
    EBNF letter     = unicode_letter | "_" .
    ```
- operators and punctuation
    constant strings (eg: "==")
- literals
    - Integer literals:
    ```
    EBNF int_lit        = decimal_lit | binary_lit | octal_lit | hex_lit .
    EBNF decimal_lit    = "0" | ( "1" … "9" ) [ [ "_" ] decimal_digits ] .
    EBNF binary_lit     = "0" ( "b" | "B" ) [ "_" ] binary_digits .
    EBNF octal_lit      = "0" [ "o" | "O" ] [ "_" ] octal_digits .
    EBNF hex_lit        = "0" ( "x" | "X" ) [ "_" ] hex_digits .
    EBNF decimal_digits = decimal_digit { [ "_" ] decimal_digit } .
    EBNF binary_digits  = binary_digit { [ "_" ] binary_digit } .
    EBNF octal_digits   = octal_digit { [ "_" ] octal_digit } .
    EBNF hex_digits     = hex_digit { [ "_" ] hex_digit } .
    decimal_digit = "0" … "9" .
    binary_digit  = "0" | "1" .
    octal_digit   = "0" … "7" .
    hex_digit     = "0" … "9" | "A" … "F" | "a" … "f" .
    ```
    
    - Floating point literals
    ```
    EBNF float_lit         = decimal_float_lit | hex_float_lit .
    EBNF decimal_float_lit = decimal_digits "." [ decimal_digits ] [decimal_exponent ] | 
    						literals"." decimal_digits [ decimal_exponent ] .
    EBNF decimal_exponent  = ( "e" | "E" ) [ "+" | "-" ] decimal_digits .
    EBNF hex_float_lit     = "0" ( "x" | "X" ) hex_mantissa hex_exponent .
    EBNF hex_mantissa      = [ "_" ] hex_digits "." [ hex_digits ] |
						    [ "_" ] hex_digits | 
						    "." hex_digits .
    EBNF hex_exponent      = ( "p" | "P" ) [ "+" | "-" ] decimal_digits .
    ```
    - Imaginary literals
    ```
    EBNF imaginary_lit = (decimal_digits | int_lit | float_lit) "i" .
    ```
    - Rune literals
    ```
    EBNF rune_lit         = "'" ( unicode_value | byte_value ) "'" .
    EBNF unicode_value    = unicode_char | little_u_value | big_u_value | escaped_char .
    EBNF byte_value       = octal_byte_value | hex_byte_value .
    EBNF octal_byte_value = `\` octal_digit octal_digit octal_digit .
    EBNF hex_byte_value   = `\` "x" hex_digit hex_digit .
    EBNF little_u_value   = `\` "u" hex_digit hex_digit hex_digit hex_digit .
    EBNF big_u_value      = `\` "U" hex_digit hex_digit hex_digit hex_digit hex_digit hex_digit hex_digit hex_digit .
    EBNF escaped_char     = `\` ( "a" | "b" | "f" | "n" | "r" | "t" | "v" | `\` | "'" | `"` ) .
    ```
    - String literals
    ```
    string_lit             = raw_string_lit | interpreted_string_lit .
    raw_string_lit         = "`" { unicode_char | newline } "`" .
    interpreted_string_lit = `"` { unicode_value | byte_value } `"` .
    ```
    
- semicolons (skip)
- newline (update row and column)
- whitespace (update column)
- any other character (show error)

## Project requirements (RO):

Analizator lexical (manual sau automat)

Pentru prima tema trebuie sa scrieti un analizor lexical pentru un limbaj de programare la alegere. 

Scrieti analizorul sub forma unei functii care returneaza:

```
- tipul token-ului curent
- lungimea sirului corespunzator din fisierul de intrare
- linia din fisierul de intrare pe care se afla token-ul curent
- pointer catre primul caracter al token-ulului curent
- un mesaj de eroare atunci cand este intalnita o eroare lexicala.
```

Functia este apelata din programul principal, in care este citit un fisier de intrare care va fi scanat cu ajutorul acestei functii, astfel incat sa se afiseze toti token-ii care apar in fisierul de intrare. 

Atunci cand este apelata, functia de scanare:

- incepand de la pointerul curent (care initial indica catre primul caracter al fisierului de intrare) sare peste un nr de caractere egal cu lungimea token-ului anterior (initial aceasta lungime este 0);
- sare peste spatii, tab-uri, linii noi, pana intalneste primul caracter diferit de acestea; 
- seteaza pointerul curent astfel ca sa indice catre acest caracter;
- identifica token-ul curent, ce corespunde sirului ce incepe cu caracterul depistat la pasul anterior; 
- determina tipul acestuia si lungimea sirului corespunzator;
- in cazul in care este intalnita o eroare lexicala, semneleaza aceasta printr-un mesaj, scaneaza fisierul de intrare in continuare, pana gaseste primul caracter de tip spatiu, linie noua, tab, seteaza pointerul curent catre acest caracter, seteza lungimea token-ului curent cu 0 (in felul acesta programul va afisa in continuare token-ii urmatori, fara sa se opreasca la prima eroare intalnita).
- se opreste cu scanarea cand a intalnit sfarsitul fisierului de intrare.

Pentru cazul automat trebuie doar sa returnati pentru fiecare token structura de mai sus.

EN: TODO

