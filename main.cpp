#include <iostream>
#include <fstream>
#include <regex>

using namespace std;

/** Scrieti analizorul sub forma unei functii care returneaza:
 * tipul token-ului curent
 * lungimea sirului corespunzator din fisierul de intrare
 * linia din fisierul de intrare pe care se afla token-ul curent
 * pointer catre primul caracter al token-ulului curent
 * un mesaj de eroare atunci cand este intalnita o eroare lexicala.
*/

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

regex keyword_exp("(break)|(case)|(chan)|(const)|(continue)|(default)|(defer)|(else)|(fallthrough)|(for)|(func)|(go)|(goto)|(if)|(import)|(interface)|(map)|(package)|(range)|(return)|(select)|(struct)|(switch)|(type)|(var)");
regex operator_exp("(<<=)|(>>=)|(\\.\\.\\.)|(&\\^=)|(\\+=)|(&=)|(&&)|(==)|(!=)|(-=)|(\\|=)|(\\|\\|)|(<=)|(\\*=)|(\\^=)|(<-)|(>=)|(<<)|(\\/=)|(\\+\\+)|(:=)|(>>)|(%=)|(--)|(&\\^)|(\\+)|(\\&)|(\\()|(\\))|(-)|(\\|)|(<)|(\\[)|(\\])|(\\*)|(\\^)|(>)|(\\{)|(\\})|(\\/)|(=)|(,)|(;)|(%)|(!)|(\\.)|(:)");
regex identifier_exp("([a-zA-Z_])(([a-zA-Z_])|\\d)*");

vector<regex> dgt = {
    //dec
    regex("\\d(_?\\d)*"),
    //bin
    regex("[01](_?[01])*"),
    //oct
    regex("[0-7](_?[0-7])*"),
    //hex
    regex("[\\d(A-F)(a-f)](_?[\\d(A-F)(a-f)])*")};

vector<regex> literal_exp = {
    dgt[0],
    dgt[1],
    dgt[2],
    dgt[3]};

string read_mlc(ifstream &code, string selection, unsigned long &rows_consumed, unsigned long &next_col)
{
    size_t mlc_end = selection.find("*/");
    if (mlc_end != string::npos)
    {
        if (selection.length() - 2 > mlc_end)
        {
            // MLC does not end with a newline
            // so we have to move the cursor
            int back_diff = selection.length() - 2 - mlc_end;
            code.seekg(-1 * back_diff, code.cur);

            rows_consumed--;
            next_col = mlc_end + 2;
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

        return read_mlc(code, selection, rows_consumed, next_col);
    }
}

token_stats generate_match_token(string s, streampos c, smatch match, unsigned long &rc, unsigned long &nc, unsigned int &row, unsigned int &col, ifstream &code)
{
    int back_diff = s.length() - match.str().length();
    if (back_diff > 0)
    {
        rc--;
        nc = match.str().length();
        code.seekg(-1 * back_diff, code.cur);
        s = match.str();
    }

    token_stats new_token("", c, s, row, col);

    return new_token;
}

token_stats lex(ifstream &code, unsigned int &row, unsigned int &column)
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
    code >> ws;

    // Step 2
    streampos cursor = code.tellg();

    string selection;
    getline(code, selection);

    unsigned long rows_consumed = 1;
    unsigned long next_col = 0;

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
        selection = read_mlc(code, selection, rows_consumed, next_col);

        token_stats new_token("MLC", cursor, selection, row, column);

        row += rows_consumed;
        column = next_col;

        return new_token;
    }

    // Step 5

    // Search for a whitespace on the row
    size_t word_end = selection.find(" ");
    if (word_end != string::npos)
    {
        // move the cursor backwards
        int back_diff = selection.length() - word_end;
        code.seekg(-1 * back_diff, code.cur);

        selection = selection.substr(0, word_end);
        rows_consumed--;
        next_col = word_end;
    }
    else
    {
        // No whitespace found, search for a tab
        size_t word_end = selection.find("\t");
        if (word_end != string::npos)
        {
            // move the cursor backwards
            // "\t" is actually one ASCII character
            int back_diff = selection.length() - word_end;
            code.seekg(-1 * back_diff, code.cur);

            selection = selection.substr(0, word_end);
            rows_consumed--;
            next_col = word_end;
        }
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

    // Step 7
    if (match_max != "")
    {
        int back_diff = selection.length() - match_max.length();
        if (back_diff > 0)
        {
            rows_consumed--;
            next_col = match_max.length();
            code.seekg(-1 * back_diff, code.cur);
            selection = match.str();
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
        next_col = wspos;
        rows_consumed--;
        code.seekg(selection.length() - exl, code.cur);
    }
    token_stats new_token(cursor, selection, row, column, "lex-error");

    row += rows_consumed;
    column = next_col;

    return new_token;
}

int main(int argc, char **argv)
{
    ifstream code;
    unsigned int row, column;

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

            while (code.peek() != EOF)
            {
                token_stats latest_token = lex(code, row, column);

                if (latest_token.errm == "")
                {
                    cout << latest_token.pointer
                         << "\t" << latest_token.token
                         << "\t'" << latest_token.selection << "'"
                         << endl;
                }
                else
                {
                    cout << latest_token.pointer
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