/*******************************************
*  @file  CommandParserBuffer.cpp          * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "component/CommandParserBuffer.h"

CommandParserBuffer::CommandParserBuffer() {
    m_buffer["SetLogicParser"] = new SetLogicParser();
    m_buffer["SetInfoParser"] = new SetInfoParser();
    m_buffer["CommandParser"] = new CommandParser(); 
    m_buffer["DeclSortParser"] = new DeclSortParser(); 
    m_buffer["DeclDatatypesParser"] = new DeclDatatypesParser();
    m_buffer["DeclHeapParser"] = new DeclHeapParser(); 
    m_buffer["DefineFunctionParser"] = new DefineFunctionParser(); 
    m_buffer["DefineFunctionRecParser"] = new DefineFunctionRecParser(); 
    m_buffer["DeclConstParser"] = new DeclConstParser(); 
    m_buffer["AssertParser"] = new AssertParser(); 
    m_buffer["CheckSatParser"] = new CheckSatParser(); 

}

CheckSatParser* CommandParserBuffer::getCheckSatParser() {
    return dynamic_cast<CheckSatParser*>(m_buffer["CheckSatParser"]);
}

DeclConstParser* CommandParserBuffer::getDeclConstParser() {
    return dynamic_cast<DeclConstParser*>(m_buffer["DeclConstParser"]);
}

AssertParser* CommandParserBuffer::getAssertParser() {
    return dynamic_cast<AssertParser*>(m_buffer["AssertParser"]);
}

DefineFunctionParser* CommandParserBuffer::getDefineFunctionParser() {
    return dynamic_cast<DefineFunctionParser*>(m_buffer["DefineFunctionParser"]);
}

DefineFunctionRecParser* CommandParserBuffer::getDefineFunctionRecParser() {
    return dynamic_cast<DefineFunctionRecParser*>(m_buffer["DefineFunctionRecParser"]);
}

DeclHeapParser* CommandParserBuffer::getDeclHeapParser() {
    return dynamic_cast<DeclHeapParser*>(m_buffer["DeclHeapParser"]);
}

DeclDatatypesParser* CommandParserBuffer::getDeclDatatypesParser() {
    return dynamic_cast<DeclDatatypesParser*>(m_buffer["DeclDatatypesParser"]);
}

DeclSortParser* CommandParserBuffer::getDeclSortParser() {
    return dynamic_cast<DeclSortParser*>(m_buffer["DeclSortParser"]);
}

CommandParser* CommandParserBuffer::getCommandParser() {
    return m_buffer["CommandParser"];
}

SetLogicParser* CommandParserBuffer::getSetLogicParser() {
    return dynamic_cast<SetLogicParser*>(m_buffer["SetLogicParser"]);
}

SetInfoParser* CommandParserBuffer::getSetInfoParser() {
	return dynamic_cast<SetInfoParser*>(m_buffer["SetInfoParser"]);
}

CommandParserBuffer::~CommandParserBuffer() {
    for (auto item : m_buffer) {
        if (item.second != nullptr)
            delete item.second;
    }
}
