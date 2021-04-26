#include <fstream>
#include <iomanip>
#include "Scanner.h"
#include "TokenScannerFactory.h"
#include <cstdlib>

using std::ifstream;
using std::cout;
using std::endl;
using std::string;

void testTokenScanner(string file_name) {
    ifstream fin;
    fin.open(file_name, std::ios_base::binary);
    if (!fin.is_open()) {
        cout << "OPEN FILE FAILED!\n";
        exit(-1);
    }


    Scanner scanner(fin);

    TokenScanner::initNormalizedTable();
    TokenScannerFactory factory;

    TokenScanner* ts = nullptr;
    Token* result = nullptr;
    char sign = 0;
    while(scanner.skip()) {

        sign = TokenScanner::id(scanner.curr());
        ts = factory.getTokenScanner(sign);
        result = ts->scan(scanner);
        if (result->type() != EOF_TOKEN)
            result->display();
        cout << endl;
    }

    fin.close();

}

int main(int argc, const char *argv[])
{
    if (argc == 2) {
        string file_name = argv[1];
        testTokenScanner(file_name);
    } 
    return 0;
}

