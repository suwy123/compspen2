/*******************************************
*  @file  CommandParserFactory.cpp         * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "CommandParserFactory.h"

/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @param sign Parameter description
 * @return Return parameter description
 */
CommandParser* CommandParserFactory::getCommandParser(const string& sign) {
    
    if (sign == "set-logic") {
        return m_buffer.getSetLogicParser(); 
    } else if (sign == "set-info") {
        return m_buffer.getSetInfoParser();
    } else if (sign == "declare-sort") {
        return m_buffer.getDeclSortParser();
    } else if (sign == "declare-datatypes") {
        return m_buffer.getDeclDatatypesParser();
    } else if (sign == "declare-heap") {
        return m_buffer.getDeclHeapParser();
    }  else if (sign == "define-fun") {
        return m_buffer.getDefineFunctionParser();
    }  else if (sign == "define-fun-rec") {
        return m_buffer.getDefineFunctionRecParser();
    } else if (sign == "declare-const") {
        return m_buffer.getDeclConstParser();
    } else if (sign == "assert") {
        return m_buffer.getAssertParser();
    } else if (sign == "check-sat") {
        return m_buffer.getCheckSatParser();
    }

    return nullptr;
}
