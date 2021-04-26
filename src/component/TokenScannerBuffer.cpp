/*******************************************
*  @file  TokenScannerBuffer.cpp           * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "component/TokenScannerBuffer.h"

TokenScannerBuffer::TokenScannerBuffer() {
    m_buffer["TokenScanner"] = new TokenScanner();
    m_buffer["CommentScanner"] = new CommentScanner();
    m_buffer["KeywordScanner"] = new KeywordScanner();
    m_buffer["LeftParenScanner"] = new LeftParenScanner();
    m_buffer["NumberLiteralScanner"] = new NumberLiteralScanner();
    m_buffer["RightParenScanner"] = new RightParenScanner();
    m_buffer["StringLiteralScanner"] = new StringLiteralScanner();
    m_buffer["SymbolScanner"] = new SymbolScanner();
}

TokenScanner* TokenScannerBuffer::getTokenScanner() {
    return m_buffer["TokenScanner"];
}

CommentScanner* TokenScannerBuffer::getCommentScanner() {
    return  dynamic_cast<CommentScanner*>(m_buffer["CommentScanner"]);
}

KeywordScanner* TokenScannerBuffer::getKeywordScanner() {
    return dynamic_cast<KeywordScanner*>(m_buffer["KeywordScanner"]);
}

LeftParenScanner* TokenScannerBuffer::getLeftParenScanner() {
   return dynamic_cast<LeftParenScanner*>(m_buffer["LeftParenScanner"]);
}
   
NumberLiteralScanner* TokenScannerBuffer::getNumberLiteralScanner() {
    return dynamic_cast<NumberLiteralScanner*>(m_buffer["NumberLiteralScanner"]);
}

RightParenScanner* TokenScannerBuffer::getRightParenScanner() {
    return dynamic_cast<RightParenScanner*>(m_buffer["RightParenScanner"]);
}

StringLiteralScanner* TokenScannerBuffer::getStringLiteralScanner(){
    return dynamic_cast<StringLiteralScanner*>(m_buffer["StringLiteralScanner"]);
}

SymbolScanner* TokenScannerBuffer::getSymbolScanner() {
    return dynamic_cast<SymbolScanner*>(m_buffer["SymbolScanner"]);
}


TokenScannerBuffer::~TokenScannerBuffer() {
    for (auto& item : m_buffer) {
        if (item.second != nullptr)
            delete item.second;
    }
}

