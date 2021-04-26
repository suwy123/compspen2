/*******************************************
*  @file  TokenBuffer.cpp                  * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "component/TokenBuffer.h"

/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @return Return parameter description
 */
TokenBuffer::TokenBuffer() {
    m_buffer["Token"] = new Token(NULL_TOKEN, -1, -1);
    m_buffer["StrToken"] = new StrToken(STRING_TOKEN, -1, -1, "");
    m_buffer["FloatToken"] = new FloatToken(FLOAT_TOKEN, -1, -1, 0.0f);
    m_buffer["IntToken"] = new IntToken(INT_TOKEN, -1, -1, 0);

}

TokenBuffer::~TokenBuffer() {
    for (auto& item : m_buffer) {
        if (item.second != nullptr)
            delete item.second;
    }
}
