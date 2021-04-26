#if !defined(TOKEN_SCANNER_BUFFER_)
#define TOKEN_SCANNER_BUFFER_
/*******************************************
*  @file  TokenScannerBuffer.h             * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "TokenScanner.h"
#include "scanner/LeftParenScanner.h"
#include "scanner/RightParenScanner.h"
#include "scanner/CommentScanner.h"
#include "scanner/KeywordScanner.h"
#include "scanner/NumberLiteralScanner.h"
#include "scanner/StringLiteralScanner.h"
#include "scanner/SymbolScanner.h"

#include <map>

using std::map;

/*! @class TokenScannerBuffer
 *  @brief Brief class description
 *
 *  Detailed description
 */
class TokenScannerBuffer
{
public:
    TokenScannerBuffer(); 

    TokenScanner* getTokenScanner();
    CommentScanner* getCommentScanner();
    KeywordScanner* getKeywordScanner();
    LeftParenScanner* getLeftParenScanner();
    NumberLiteralScanner* getNumberLiteralScanner();
    RightParenScanner* getRightParenScanner();
    StringLiteralScanner* getStringLiteralScanner();
    SymbolScanner* getSymbolScanner();

    virtual ~TokenScannerBuffer();

protected:
    map<string, TokenScanner*> m_buffer; ///< Member description
};


#endif
