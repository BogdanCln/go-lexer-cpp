#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

struct token_stats
{
    string token;
    unsigned int length;
    unsigned long row;
    unsigned long column;
    streampos pointer;
    string errm = "";
    string selection;

    token_stats()
    {
        length = 0;
    }

    token_stats(string t, streampos p, string sel, unsigned int r, unsigned int c)
    {
        token = t;
        pointer = p;
        length = sel.length();
        selection = sel;
        row = r;
        column = c;
    }

    token_stats(streampos p, string sel, unsigned int r, unsigned int c, string err)
    {
        pointer = p;
        length = sel.length();
        selection = sel;
        row = r;
        column = c;
        errm = err;
    }
};

struct EndException : public exception
{
    const char *what() const throw()
    {
        return "Reached the end of the file";
    }
};

regex keyword_exp("(break)|(case)|(chan)|(const)|(continue)|(default)|(defer)|(else)|(fallthrough)|(for)|(func)|(go)|(goto)|(if)|(import)|(interface)|(map)|(package)|(range)|(return)|(select)|(struct)|(switch)|(type)|(var)");
regex identifier_exp("([a-zA-Z_])(([a-zA-Z_])|\\d)*");
regex operator_exp("(<<=)|(>>=)|(\\.\\.\\.)|(&\\^=)|(\\+=)|(&=)|(&&)|(==)|(!=)|(-=)|(\\|=)|(\\|\\|)|(<=)|(\\*=)|(\\^=)|(<-)|(>=)|(<<)|(\\/=)|(\\+\\+)|(:=)|(>>)|(%=)|(--)|(&\\^)|(\\+)|(\\&)|(\\()|(\\))|(-)|(\\|)|(<)|(\\[)|(\\])|(\\*)|(\\^)|(>)|(\\{)|(\\})|(\\/)|(=)|(,)|(;)|(%)|(!)|(\\.)|(:)");

string dgt[4] = {
    // EBNF decimal_digits = decimal_digit { [ "_" ] decimal_digit } .
    "(\\d(_?\\d)*)",

    // EBNF binary_digits  = binary_digit { [ "_" ] binary_digit } .
    "([01](_?[01])*)",

    // EBNF octal_digits   = octal_digit { [ "_" ] octal_digit } .
    "([0-7](_?[0-7])*)",

    // EBNF hex_digits     = hex_digit { [ "_" ] hex_digit } .
    "((\\d|[A-F]|[a-f])(_?(\\d|[A-F]|[a-f]))*)"};

string int_lits[4] = {
    // EBNF decimal_lit    = "0" | ( "1" … "9" ) [ [ "_" ] decimal_digits ] .
    "(0|\\d(_?" + dgt[0] + ")?)",

    // EBNF binary_lit     = "0" ( "b" | "B" ) [ "_" ] binary_digits .
    "(0[bB]_?" + dgt[1] + ")",

    // EBNF octal_lit      = "0" [ "o" | "O" ] [ "_" ] octal_digits .
    "(0[oO]?_?" + dgt[2] + ")",

    // EBNF hex_lit        = "0" ( "x" | "X" ) [ "_" ] hex_digits .
    "(0[xX]_?" + dgt[3] + ")",
};

regex literal_integer_exp[8] = {
    regex(dgt[0]),
    regex(dgt[1]),
    regex(dgt[2]),
    regex(dgt[3]),

    //  EBNF int_lit       = decimal_lit | binary_lit | octal_lit | hex_lit .

    /**
     * Wrong: regex(int_lits[0] + "|" + int_lits[1] + "|" + int_lits[2] + "|" + int_lits[3])
     * Need to match the longer one so we need separare regexes
    **/
    regex(int_lits[0]),
    regex(int_lits[1]),
    regex(int_lits[2]),
    regex(int_lits[3]),
};

string exp[2] = {
    // EBNF decimal_exponent  = ( "e" | "E" ) [ "+" | "-" ] decimal_digits .
    "([eE][+-]?" + dgt[0] + ")",
    // EBNF hex_exponent      = ( "p" | "P" ) [ "+" | "-" ] decimal_digits .
    "([pP][+-]?" + dgt[0] + ")",
};

// EBNF hex_mantissa = [ "_" ] hex_digits "." [ hex_digits ] |[ "_" ] hex_digits |"." hex_digits .
string hex_man = "((_?" + dgt[3] + "\\." + dgt[3] + "?)|(_?" + dgt[3] + ")|(\\." + dgt[3] + "))";

string float_lits[4] = {
    // EBNF decimal_float_lit = decimal_digits "." [ decimal_digits ] [ decimal_exponent ] | decimal_digits decimal_exponent | "." decimal_digits [ decimal_exponent ] .
    /**
     * Wrong: "((" + dgt[0] + "\\.(" + dgt[0] + ")?(" + exp[0] + ")?)|(" + dgt[0] + exp[0] + ")|(\\." + dgt[0] + "(" + exp[0] + ")?))",
     * Need to match the longer one so we need separare regexes
    **/
    // decimal_digits "." [ decimal_digits ] [ decimal_exponent ]
    "(" + dgt[0] + "\\.(" + dgt[0] + ")?(" + exp[0] + ")?)",
    // decimal_digits decimal_exponent
    "(" + dgt[0] + exp[0] + ")",
    // "." decimal_digits [ decimal_exponent ]
    "(\\." + dgt[0] + "(" + exp[0] + ")?)",

    // EBNF hex_float_lit     = "0" ( "x" | "X" ) hex_mantissa hex_exponent .
    "(0[xX]" + hex_man + exp[1] + ")",
};

regex literal_float_exp[7] = {
    // EBNF decimal_exponent
    regex(exp[0]),

    // EBNF float_lit         = decimal_float_lit | hex_float_lit .
    /**
     * Wrong: regex(float_lits[0] + "|" + float_lits[1]+ "|" + float_lits[2] + "|" + float_lits[3]),
     * Need to match the longer one so we need separare regexes
    **/
    regex(float_lits[0]),
    regex(float_lits[1]),
    regex(float_lits[2]),
    regex(float_lits[3]),

    // EBNF hex_mantissa
    regex(hex_man),

    // EBNF hex_exponent
    regex(exp[1]),
};

// EBNF imaginary_lit = (decimal_digits | int_lit | float_lit) "i" .
regex literal_imaginary_lit("((" + dgt[0] + ")|(" + int_lits[0] + "|" + int_lits[1] + "|" + int_lits[2] + "|" + int_lits[3] + ")|(" + float_lits[0] + "|" + float_lits[1]+ "|" + float_lits[2] + "|" + float_lits[3] + "))i");

string read_mlc(ifstream &code, string selection, unsigned long &rows_consumed, unsigned long &next_col, unsigned long start_col)
{
    size_t mlc_end = selection.find("*/");
    if (mlc_end != string::npos)
    {
        if (selection.length() - 2 > mlc_end)
        {
            // MLC does not end with a newline
            // so we have to move the cursor
            // int back_diff = selection.length() - 2 - mlc_end;
            // code.seekg(-1 * back_diff, code.cur);

            if (rows_consumed >= 1)
                rows_consumed--;
            // next_col = mlc_end + 2;
            next_col = start_col + mlc_end + 2;

            selection = selection.substr(0, mlc_end + 2);
        }

        return selection;
    }
    else
    {
        string line;
        getline(code, line);

        selection = selection + "\n" + line;
        rows_consumed++;
        next_col = 0;

        return read_mlc(code, selection, rows_consumed, next_col, 0);
    }
}

token_stats lex(ifstream &code, unsigned int &row, unsigned int &column, int skip_count)
{
    /**
     * 1. skip whitespaces, tabs and newlines
     * 2. selection = content from current pointer up until the first newline
     * 3. Check if we have a single line comment = line starts with //
     *    True  => update row, column; return token_stats with current selection
     *    False => GO TO 3
     * 4. Check if we have a multi line comment = line starts with /*
     *    True  => update selection to include all input until end of multi line comment
     *             (can be a trim of current line or multiple lines)
     *             update row, column; return token_stats with current selection
     *    False => GO TO 4
     * 5. Trim selection to include only the first word (0 to first whitespace)
     * 6. Run all the regular expressions with the following rules:
     *    6.1. Take in consideration only matches that start from the first character
     *    6.2. Keep in memory the longest match so far
     *    6.3. If there is a match for the whole selection stop and GO TO 8
     * 7. If no regular expression matched 
     *     then GO TO 8
     *     else selection = match; update cursor, row, column; return token_stats with the match
     * 8. update row, column; return token_stats with current selection and an error message
     */

    // Step 1
    unsigned long ws_count = 0;

    // start_col is needed for accurate next_col calculation
    unsigned long start_col = column;

    // skip shitespaces, tabs and newlines
    // code >> ws;

    const string skip = " \t\n";
    while (true)
    {
        int next_c = code.peek();
        if (string::npos != skip.find(next_c))
        {
            if ((char)next_c == '\n')
            {
                start_col = 0;
                row++;
            }
            else
            {
                ws_count++;
                start_col++;
            }

            code.ignore();
        }
        else
        {
            break;
        }
    }

    if (code.peek() == EOF)
        throw EndException();

    streampos cursor = code.tellg();

    // Step 2
    string selection;
    getline(code, selection);
    unsigned long rows_consumed = 1;
    unsigned long next_col = 0;

    // set the cursor back the the beggining of the selection
    code.seekg(cursor);

    // Step 3
    if (selection[0] == '/' && selection[1] == '/')
    {
        token_stats new_token("SLC", cursor, selection, row, column);

        row += rows_consumed;
        column = next_col;

        return new_token;
    }

    // Step 4
    if (selection[0] == '/' && selection[1] == '*')
    {
        selection = read_mlc(code, selection, rows_consumed, next_col, start_col);
        code.seekg(cursor);

        token_stats new_token("MLC", cursor, selection, row, column);

        row += rows_consumed;
        column = next_col;

        return new_token;
    }

    // Step 5

    // Search for a whitespace on the row
    size_t first_ws = selection.find(" ");
    size_t first_tab = selection.find("\t");
    size_t word_end = (first_ws < first_tab) ? first_ws : first_tab;
    if (word_end != string::npos && word_end != selection.length() - 1)
    {
        selection = selection.substr(0, word_end);

        if (rows_consumed >= 1)
            rows_consumed--;

        next_col = start_col + selection.length();
    }

    // Step 6
    smatch match;
    string match_max = "";
    string token_max = "";

    // Keyword tokens
    if (regex_search(selection, match, keyword_exp, regex_constants::match_continuous))
    {
        string matchstr = match.str();
        if (matchstr.length() == selection.length())
        {
            token_stats new_token("keyword", cursor, selection, row, column);

            row += rows_consumed;
            column = next_col;
            return new_token;
        }
        else if (matchstr.length() > match_max.length())
        {
            match_max = matchstr;
            token_max = "keyword";
        }
    }

    // Identifier tokens
    if (regex_search(selection, match, identifier_exp, regex_constants::match_continuous))
    {
        string matchstr = match.str();
        if (matchstr.length() == selection.length())
        {
            token_stats new_token("identifier", cursor, selection, row, column);

            row += rows_consumed;
            column = next_col;
            return new_token;
        }
        else if (matchstr.length() > match_max.length())
        {
            match_max = matchstr;
            token_max = "identifier";
        }
    }

    // Operator/punctuation tokens
    if (regex_search(selection, match, operator_exp, regex_constants::match_continuous))
    {
        string matchstr = match.str();
        if (matchstr.length() == selection.length())
        {
            token_stats new_token("operator", cursor, selection, row, column);

            row += rows_consumed;
            column = next_col;
            return new_token;
        }
        else if (matchstr.length() > match_max.length())
        {
            match_max = matchstr;
            token_max = "operator";
        }
    }

    // Literals
    // Integer literals
    for (int i = 0; i < 8; i++)
    {
        if (regex_search(selection, match, literal_integer_exp[i], regex_constants::match_continuous))
        {
            string matchstr = match.str();
            if (matchstr.length() == selection.length())
            {
                token_stats new_token("integer_lit", cursor, selection, row, column);

                row += rows_consumed;
                column = next_col;
                return new_token;
            }
            else if (matchstr.length() > match_max.length())
            {
                match_max = matchstr;
                token_max = "integer_lit";
            }
        }
    }

    // Floating-point literals
    for (int i = 0; i < 7; i++)
    {
        if (regex_search(selection, match, literal_float_exp[i], regex_constants::match_continuous))
        {
            string matchstr = match.str();
            if (matchstr.length() == selection.length())
            {
                token_stats new_token("float_lit", cursor, selection, row, column);

                row += rows_consumed;
                column = next_col;
                return new_token;
            }
            else if (matchstr.length() > match_max.length())
            {
                match_max = matchstr;
                token_max = "float_lit";
            }
        }
    }

    // Imaginary literals
    if (regex_search(selection, match, literal_imaginary_lit, regex_constants::match_continuous))
    {
        string matchstr = match.str();
        if (matchstr.length() == selection.length())
        {
            token_stats new_token("imaginary_lit", cursor, selection, row, column);

            row += rows_consumed;
            column = next_col;
            return new_token;
        }
        else if (matchstr.length() > match_max.length())
        {
            match_max = matchstr;
            token_max = "imaginary_lit";
        }
    }
    // Step 7
    if (match_max != "")
    {
        int back_diff = selection.length() - match_max.length();
        if (back_diff > 0)
        {
            if (rows_consumed >= 1)
                rows_consumed--;

            next_col = start_col + match_max.length();
            selection = match_max;
        }

        token_stats new_token(token_max, cursor, selection, row, column);

        row += rows_consumed;
        column = next_col;

        return new_token;
    }

    // Step 8
    int exl = selection.length();
    size_t wspos = selection.find(" ");
    if (wspos != string::npos)
    {
        selection = selection.substr(0, wspos);

        if (rows_consumed >= 1)
            rows_consumed--;

        next_col = start_col + wspos;
    }
    token_stats new_token(cursor, selection, row, column, "lex-error");

    row += rows_consumed;
    column = next_col;

    return new_token;
}

int main(int argc, char **argv)
{
    ifstream code;
    unsigned int row = 1, column = 1;

    if (argc < 2)
    {
        cout << "\nPlease specify the go source file path as command line parameter!\n";
        return 0;
    }
    else
    {
        code.open(argv[1]);
        if (code.is_open())
        {
            cout << "Succesfully opened " << argv[1] << endl;
            token_stats latest_token;

            while (true)
            {
                if (latest_token.length > 0)
                {
                    code.seekg(latest_token.length, code.cur);
                    if (code.peek() == '\n')
                    {
                        code.ignore();
                        column = 0;
                    }
                }

                try
                {
                    latest_token = lex(code, row, column, latest_token.length);
                    /* code */
                }
                catch (const exception &e)
                {
                    cerr << e.what() << endl;
                    break;
                }

                if (latest_token.errm == "")
                {
                    cout << latest_token.row << " | " << latest_token.column
                         << "\t" << latest_token.token
                         << "\t'" << latest_token.selection << "'"
                         << endl;
                }
                else
                {
                    cout << latest_token.row << " | " << latest_token.column
                         << "\t" << latest_token.errm
                         << "\t'" << latest_token.selection << "'"
                         << endl;
                }
            }
        }
        else
        {
            cout << "Couldn't open " << argv[1] << endl;
            return 0;
        }
    }

    return 0;
}