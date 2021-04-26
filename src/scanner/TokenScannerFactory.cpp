/*******************************************
*  @file  TokenScannerFactory.cpp          * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "TokenScannerFactory.h"

/*! @brief Build TokenScanner
 *
 *  Detailed description
 *
 * @param sign The first character of Token
 * @return Return the TokenScanner 
 */
TokenScanner* TokenScannerFactory::getTokenScanner(char sign) {
    switch(sign) {
        case '(':
            return m_buffer.getLeftParenScanner();
        case ')':
            return m_buffer.getRightParenScanner();
        case '0':
        case '-':
            return m_buffer.getNumberLiteralScanner();
        case '|':
        case 'a':
            return m_buffer.getSymbolScanner();
        case '"':
            return m_buffer.getStringLiteralScanner();
        case ';':
            return m_buffer.getCommentScanner();
        case ':':
            return m_buffer.getKeywordScanner();
        default:
            return m_buffer.getTokenScanner();
    }
}

