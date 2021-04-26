#if !defined(TOKEN_SCANNER_FACTORY_)
#define TOKEN_SCANNER_FACTORY_
/*******************************************
*  @file  TokenScannerFactory              *
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-8                     *
*                                          *
*******************************************/

#include "component/TokenScannerBuffer.h"
extern TokenScannerBuffer token_scanner_buffer;

/*! @class TokenScannerFactory
 *  @brief Brief class description
 *
 *  Detailed description
 */
class TokenScannerFactory
{
public:
    TokenScannerFactory():m_buffer(token_scanner_buffer) {}
    virtual ~TokenScannerFactory() {}
    TokenScanner* getTokenScanner(char sign);
protected:
    TokenScannerBuffer& m_buffer;

};

#endif
