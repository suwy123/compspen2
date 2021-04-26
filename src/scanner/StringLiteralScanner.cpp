/*******************************************
*  @file  StringLiteralScanner.cpp         * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "scanner/StringLiteralScanner.h"

/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @param scanner Parameter description
 * @return Return parameter description
 */
Token* StringLiteralScanner::scan(Scanner& scanner) {
    // assert (scanner.curr() == '"')
    int line = scanner.line();
    int col = scanner.col();
    scanner.clearStrCache();
    while (scanner.next()) {
        if (scanner.curr() == '"') {
            StrToken* token = m_buffer.getStrToken();
            token->reset(STRING_TOKEN, line, col, 
                    string(scanner.getCache().begin(), scanner.getCache().end()));
            return token;
        }
        scanner.cache(scanner.curr());
    }
    Token* token = m_buffer.getToken();
    token->reset(EOF_TOKEN, line, col);
    return token;
}
