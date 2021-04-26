#if !defined(TOKEN_BUFFER_)
#define TOKEN_BUFFER_
/*******************************************
*  @file  TokenBuffer.h                    * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "Token.h"
#include <map>

using std::map;

/*! @class TokenBuffer
 *  @brief Brief class description
 *
 *  Detailed description
 */
class TokenBuffer
{
public:
    TokenBuffer();
    Token* getToken() {return m_buffer["Token"];}
    StrToken* getStrToken() {return dynamic_cast<StrToken*>(m_buffer["StrToken"]);}
    FloatToken* getFloatToken() {return dynamic_cast<FloatToken*>(m_buffer["FloatToken"]);}
    IntToken* getIntToken() {return dynamic_cast<IntToken*>(m_buffer["IntToken"]);}

    virtual ~TokenBuffer();

protected:
    map<string, Token*> m_buffer; ///< Member description
};

#endif
