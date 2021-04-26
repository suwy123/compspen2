/*******************************************
*  @file  NumberLiteralScanner.cpp         * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "scanner/NumberLiteralScanner.h"


/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @param scanner Parameter description
 * @return Return parameter description
 */
Token* NumberLiteralScanner::scan(Scanner& scanner) {
    // assert(scanner.curr() == '0' || '-')
    int line = scanner.line();
    int col = scanner.col();
    scanner.clearStrCache();
    scanner.cache(scanner.curr());
    int what = 0; // 0: int, 1: float, -1: symbol
    while(scanner.next()) {
        if (stop(scanner.curr())) {
            break;
        } else if (scanner.curr() == '.') {
            if (what == 0) {
                what = 1;
            } else if (what == 1) {
                what = -1;
            }
        } else if (!isdigit(scanner.curr())) {
            what = -1;
        } 
        scanner.cache(scanner.curr());
    }


    if (scanner.iseof()) {
        Token* token = m_buffer.getToken();
        token->reset(EOF_TOKEN, line, col);
        return token;
    }

    scanner.back(-1); 

    if (scanner.getCache().size() == 1 
        && scanner.getCache()[0] == '-') what = -1;
    
    if (what == 0) {
        IntToken* token = m_buffer.getIntToken();
        token->reset(INT_TOKEN, line, col,
                std::stoi(string(scanner.getCache().begin(), 
                        scanner.getCache().end())));
        return token;
    } else if (what == 1) {
        FloatToken* token = m_buffer.getFloatToken();
        token->reset(FLOAT_TOKEN, line, col,
                std::stof(string(scanner.getCache().begin(),
                        scanner.getCache().end())));
        return token;
    } else {
        StrToken* token = m_buffer.getStrToken();
        token->reset(SYMBOL_TOKEN, line, col,
                string(scanner.getCache().begin(), 
                    scanner.getCache().end()));
        return token;
    }
}

/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @param curr Parameter description
 * @return Return parameter description
 */
bool NumberLiteralScanner::stop(char curr) {
    set<char> stopset;
    stopset.insert(' ');
    stopset.insert(')');
    stopset.insert('(');
    stopset.insert('\n');
    return stopset.find(m_normalized_table[static_cast<int>(curr)]) != stopset.end();   
}
