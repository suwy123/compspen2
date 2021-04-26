#include "Parser.h"

#include <fstream>
#include <iostream>
#include <cstdlib>

using std::ifstream;
using std::cout;
using std::endl;
using std::string;

void testParser(string& file_name) {
    ifstream fin;
    fin.open(file_name, std::ios_base::binary);
    if (!fin.is_open()) {
        cout << "OPEN FILE FAILED!" << endl;
        exit(-1);
    }

    TokenScanner::initNormalizedTable();
    TokenScannerFactory factory;

    Parser parser(fin, factory);
    try {
        parser.parse();
    } catch(std::exception& e) {
        cout << e.what() << endl;
        exit(-1);
    }

    fin.close();
}


int main(int argc, const char *argv[])
{
    if (argc == 2) {
        string file_name = argv[1];
        cout << "file: " << file_name << endl;
        testParser(file_name);
    } 
    return 0;
}
