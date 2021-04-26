#if !defined(TOKEN_SCANNER_)
#define TOKEN_SCANNER_
/*******************************************
*  @file  TokenScanner.h                   * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-8                     *
*                                          *
*******************************************/
#include "Scanner.h"
#include "Token.h"
#include "component/TokenBuffer.h"
extern TokenBuffer token_buffer;

#include <cctype>
#include <set>

using std::set;


/*! @class TokenScanner
 *  @brief Base class of all token scanners
 *
 *  Detailed description
 */
class TokenScanner
{
public:
    TokenScanner():m_buffer(token_buffer) {}
    virtual ~TokenScanner() {}
    virtual Token* scan(Scanner& scanner) {
        Token* token = m_buffer.getToken();
        token->reset(NULL_TOKEN, scanner.line(), scanner.col());
        return token;
    }
    
public:
    static void initNormalizedTable();
    static char id(char ch) {return m_normalized_table[static_cast<int>(ch)];}

protected:
    virtual bool stop(char curr) {return false;}

    TokenBuffer& m_buffer;
    static char m_normalized_table[256]; ///< normalized table for characters


};

#endif
