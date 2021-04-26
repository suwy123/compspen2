#if !defined(STRING_LITERAL_SCANNER_)
#define STRING_LITERAL_SCANNER_
/*******************************************
*  @file  StringLiteralScanner.h           * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "TokenScanner.h"

/*! @class StringLiteralScanner
 *  @brief Brief class description
 *
 *  Detailed description
 */
class StringLiteralScanner: public TokenScanner
{
public:
    StringLiteralScanner() {}
    virtual ~StringLiteralScanner() {}
    Token* scan(Scanner& scanner); 
};
#endif
