/*******************************************
*  @file  TokenScanner.cpp                 * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "TokenScanner.h"

char TokenScanner::m_normalized_table[256] = {};

/*! @brief init normalized table.
 *
 * Classify the characters into different categories. 
 *     ' ', 'a', '0', special
 *
 */
void TokenScanner::initNormalizedTable() {

    for (int i = 0; i < 256; ++i) {
        m_normalized_table[i] = (signed char) i;
    }
    m_normalized_table[static_cast<int>('\t')] = ' ';
    m_normalized_table[static_cast<int>('\r')] = ' ';
    // assert ('a' < 'z');
    for (char ch = 'b'; ch <= 'z'; ++ch) {
        m_normalized_table[static_cast<int>(ch)] = 'a';
    }
    for (char ch = 'A'; ch <= 'Z'; ++ch) {
        m_normalized_table[static_cast<int>(ch)] = 'a';
    }
    // assert ('0' < '9', '9' - '0' == 9);
    for (char ch = '1'; ch <= '9'; ++ch) {
        m_normalized_table[static_cast<int>(ch)] = '0';
    }
    // SMT2 "Symbols": ~ ! @ $ % ^ & * _ - + = < > . ? /
    m_normalized_table[static_cast<int>('~')] = 'a';
    m_normalized_table[static_cast<int>('!')] = 'a';
    m_normalized_table[static_cast<int>('@')] = 'a';
    m_normalized_table[static_cast<int>('$')] = 'a';
    m_normalized_table[static_cast<int>('%')] = 'a';
    m_normalized_table[static_cast<int>('^')] = 'a';
    m_normalized_table[static_cast<int>('&')] = 'a';
    m_normalized_table[static_cast<int>('*')] = 'a';
    m_normalized_table[static_cast<int>('_')] = 'a';
    m_normalized_table[static_cast<int>('-')] = '-';
    m_normalized_table[static_cast<int>('+')] = 'a';
    m_normalized_table[static_cast<int>('=')] = 'a';
    m_normalized_table[static_cast<int>('<')] = 'a';
    m_normalized_table[static_cast<int>('>')] = 'a';
    m_normalized_table[static_cast<int>('.')] = 'a';
    m_normalized_table[static_cast<int>('?')] = 'a';
    m_normalized_table[static_cast<int>('/')] = 'a'; 
}



