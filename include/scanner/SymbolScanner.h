#if !defined(SYMBOL_SCANNER_)
#define SYMBOL_SCANNER_
/*******************************************
*  @file  SymbolScanner.h                  * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "TokenScanner.h"
#include <cctype>

/*! @class SymbolScanner
 *  @brief Brief class description
 *
 *  Detailed description
 */
class SymbolScanner: public TokenScanner
{
public:
    SymbolScanner() {}
    virtual ~SymbolScanner() {}
    Token* scan(Scanner& scanner); 

protected:
    virtual bool stop(char curr);
};

#endif
