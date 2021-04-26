/*******************************************
*  @file  CommentScanner.cpp               * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "scanner/CommentScanner.h"


/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @param scanner Parameter description
 * @return Return parameter description
 */
Token* CommentScanner::scan(Scanner& scanner) {
    // assert(scanner.curr() == ';')
    int line = scanner.line();
    int col = scanner.col();
    Token* token = m_buffer.getToken();
    while(scanner.next()) {
        if (scanner.curr() == '\n') {
            token->reset(NULL_TOKEN, line, col); 
            return token;
        }
    }

    token->reset(EOF_TOKEN, line, col);
    return token;
}

