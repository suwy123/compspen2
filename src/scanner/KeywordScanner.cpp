/*******************************************
*  @file  KeywordScanner.cpp               * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "scanner/KeywordScanner.h"

/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @param scanner Parameter description
 * @return Return parameter description
 */
Token* KeywordScanner::scan(Scanner& scanner) {
    // assert(scanner.curr() == ':')
    int line = scanner.line();
    int col = scanner.col();

    scanner.clearStrCache();

    while (scanner.next()) {
        if (stop(scanner.curr())) {

            scanner.back(-1);

            StrToken* token = m_buffer.getStrToken();
            token->reset(KEYWORD_TOKEN, line, col, 
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
bool KeywordScanner::stop(char curr) {
    set<char> stopset;
    stopset.insert(' ');
    stopset.insert(')');
    stopset.insert('(');
    stopset.insert('\n');
    return stopset.find(m_normalized_table[static_cast<int>(curr)]) != stopset.end();
}
