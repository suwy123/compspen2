/*******************************************
*  @file  SymbolScanner.cpp                * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "scanner/SymbolScanner.h"


/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @param scanner Parameter description
 * @return Return parameter description
 */
Token* SymbolScanner::scan(Scanner& scanner) {
    // assert (scanner.curr() == 'a' || '|')
    int line = scanner.line();
    int col = scanner.col();
    scanner.clearStrCache();

    bool simple = false;
    if (scanner.curr() != '|') {
        scanner.cache(scanner.curr());
        simple = true;
    }
    

    while (scanner.next()) {
        if ((simple && stop(scanner.curr())) || 
               (!simple && scanner.curr() == '|')) {
            if (simple) scanner.back(-1);
            StrToken* token = m_buffer.getStrToken();
            token->reset(SYMBOL_TOKEN, line, col, 
                    string(scanner.getCache().begin(), scanner.getCache().end()));
            return token;
        } 
        scanner.cache(scanner.curr());
    }

    Token* token = m_buffer.getToken();
    token->reset(EOF_TOKEN, line, col);
    return token;
}


/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @param curr Parameter description
 * @return Return parameter description
 */
bool SymbolScanner::stop(char curr) {
    set<char> stopset;
    stopset.insert(' ');
    stopset.insert(')');
    stopset.insert('(');
    stopset.insert('\n');
    return stopset.find(m_normalized_table[static_cast<int>(curr)]) != stopset.end();
}
