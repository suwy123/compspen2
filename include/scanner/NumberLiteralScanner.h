#if !defined(INT_LITERAL_SCANNER_)
#define INT_LITERAL_SCANNER_
/*******************************************
*  @file  NumberLiteralScanner.h              * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-9                     *
*                                          *
*******************************************/

#include "TokenScanner.h"

/*! @class NumberLiteralScanner
 *  @brief Brief class description
 *
 *  Detailed description
 */
class NumberLiteralScanner: public TokenScanner
{
public:
    NumberLiteralScanner() {}
    virtual ~NumberLiteralScanner() {}
    Token* scan(Scanner& scanner); 
protected:
    virtual bool stop(char curr);
};

#endif
