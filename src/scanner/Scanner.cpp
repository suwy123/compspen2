/*******************************************
*  @file  Scanner.cpp                      * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "TokenScanner.h"
#include "TokenScannerFactory.h"

extern TokenScannerFactory tokenScannerFactory;

#include <iostream>
using std::cout;
using std::endl;

/**
* @brief constructor 
*
* @param in input stream 
*
*/
Scanner::Scanner(istream& in)
    :m_line(1),
    m_position(0),
    m_curr(0),
    m_is_eof(false),
    m_str_cache(0),
    m_in(in) { 

}

/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @param type Parameter description
 * @param info Parameter description
 * @return Return parameter description
 */
Token* Scanner::checkNext(TOKEN type, string info) {

    if (skip()) {
        TokenScanner* ts = nullptr;
        Token* result = nullptr;
        char sign = 0; 
        sign = TokenScanner::id(curr());
        ts = tokenScannerFactory.getTokenScanner(sign);
        result = ts->scan(*this);

        if (result->type() != type)
            throw SyntaxException(info + " got " + dynamic_cast<StrToken*>(result)->value(), result->row(), result->col());

        return result;
    }

    return nullptr;   
}

/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @return Return parameter description
 */
Token* Scanner::nextToken() {

    if (skip()) {
        TokenScanner* ts = nullptr;
        Token* result = nullptr;
        char sign = 0; 
        sign = TokenScanner::id(curr());
        ts = tokenScannerFactory.getTokenScanner(sign);
        result = ts->scan(*this);
        if (result->type() == NULL_TOKEN) {
            return nextToken();
        }
        return result;
    }

    return nullptr;
}

/**
* @brief scan next character which is not space.
*
* @return Return which is eof.
*/
bool Scanner::next() {
    m_in.get(m_curr);
    // check eof flag
    if (m_in.eof()) {
        m_is_eof = true;
        m_curr = 0;
    } else { 
        if (m_curr == '\n')
            newLine();
        else
            m_position ++;
    }
    // std::cout << m_line << "," << m_position << " -> " << m_curr << std::endl;
    return !m_is_eof;
}

/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @return Return parameter description
 */
bool Scanner::skip() {
    while(next()) {
        if (!isspace(m_curr) && m_curr != '\n') return true;
    }
    return false;
}
