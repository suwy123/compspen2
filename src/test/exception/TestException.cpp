
/*******************************************
*  @file  TestException.cpp                * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "exception/SyntaxException.h"
#include "exception/SemanticException.h"
#include <iostream>

using std::string;
using std::cout;
using std::endl;

void test(string& str) {
    if (str == "syntax") {
        throw SyntaxException("NULL");
    } else {
        throw SemanticException("EMPTY", 1, 10);
    }
}

int main(int argc, const char *argv[])
{
    string s = "syntax";
    try {
        test(s);
    } catch (BaseException& e) {
        cout << e.what() << endl;
    }
    return 0;
}
