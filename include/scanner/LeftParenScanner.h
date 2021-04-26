#if !defined(LEFT_PAREN_SCANNER_)
#define LEFT_PAREN_SCANNER_
/*******************************************
*  @file  LeftParenScanner                 * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-8                     *
*                                          *
*******************************************/
#include "TokenScanner.h"

/*! @class LeftParenScanner
 *  @brief Brief class description
 *
 *  Detailed description
 */
class LeftParenScanner: public TokenScanner
{
public:
    LeftParenScanner() {}
    virtual ~LeftParenScanner() {}
    Token* scan(Scanner& scanner) {
        Token* token = m_buffer.getToken();
        token->reset(LEFT_PAREN, scanner.line(), scanner.col());
        return token;
    }
};

#endif
