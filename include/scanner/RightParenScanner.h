#if !defined(RIGHT_PAREN_SCANNER_)
#define RIGHT_PAREN_SCANNER_
/*******************************************
*  @file  RightParenScanner                * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "TokenScanner.h"

/*! @class RightParenScanner
 *  @brief Brief class description
 *
 *  Detailed description
 */
class RightParenScanner: public TokenScanner
{
public:
    RightParenScanner() {}
    virtual ~RightParenScanner() {}
    Token* scan(Scanner& scanner) {
        Token* token = m_buffer.getToken();

        token->reset(RIGHT_PAREN, scanner.line(), scanner.col());
        return token;
    }
};
#endif
