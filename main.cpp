#include <iostream>
#include <fstream>

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

string read_mlc(ifstream &code, string selection, unsigned long &rows_consumed, unsigned long &next_col)
{
    size_t mlc_end = selection.find("*/");
    if (mlc_end != string::npos)
    {
        selection = selection.substr(0, mlc_end + 2);
        next_col = mlc_end + 2;
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

token_stats lex(ifstream &code, unsigned int &row, unsigned int &column)
{
    /**
     * 1. skip whitespaces, tabs and newlines
     * 2. selection = content from current pointer up until the first newline
     * 3. Check if we have a single line comment = line starts with //
     *    True  => update row, column; return token_stats with current selection
     *    False => GO TO 3
     * 4. Check if we have a multi line comment = line starts with /*
     *    True  => update selection to include all thext until end of multi line comment
     *             (can be a trim of current line or multiple lines)
     *             update row, column; return token_stats with current selection
     *    False => GO TO 4
     * 5. Run the regular expressions up until one gets a match that starts from the first character
     *    Found     => update row, column; return token_stats with current selection
     *    Not found => GO TO 6
     * 6. Trim selection from first character to the first whitespace, tab or new line
     *    update row, column; return token_stats with current selection and an error message
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


    // Step 6
    int exl = selection.length();
    size_t wspos = selection.find(" ");
    if (wspos != string::npos)
    {
        selection = selection.substr(0, wspos);
        next_col = wspos;
        rows_consumed--;
        // cout << exl - selection.length();
        code.seekg(selection.length() - exl, code.cur);
    }
    token_stats new_token(cursor, selection, row, column, "Lexical error");

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
                         << " - " << latest_token.token
                         << " = '" << latest_token.selection << "'"
                         << endl;
                }
                else
                {
                    cout << latest_token.pointer
                         << " - " << latest_token.errm
                         << " = '" << latest_token.selection << "'"
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