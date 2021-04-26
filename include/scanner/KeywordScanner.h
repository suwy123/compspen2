#if !defined(KEYWORD_SCANNER_)
#define KEYWORD_SCANNER_
/*******************************************
*  @file  KeywordScanner                   * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "TokenScanner.h"

/*! @class KeywordScanner
 *  @brief Brief class description
 *
 *  Detailed description
 */
class KeywordScanner: public TokenScanner
{
public:
    KeywordScanner() {}
    virtual ~KeywordScanner() {}
    Token* scan(Scanner& scanner);

protected:
    virtual bool stop(char curr);
};

#endif
