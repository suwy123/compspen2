#if !defined(COMMAND_PARSER_FACTORY_)
#define COMMAND_PARSER_FACTORY_

/*******************************************
*  @file  CommandParserFactory.h           * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "component/CommandParserBuffer.h"

extern CommandParserBuffer cmd_parser_buffer;


/*! @class CommandParserFactory
 *  @brief Brief class description
 *
 *  Detailed description
 */
class CommandParserFactory
{
public:
    CommandParserFactory() :m_buffer(cmd_parser_buffer) {}
    virtual ~CommandParserFactory() {}
    CommandParser* getCommandParser(const string& sign); 

private:
    CommandParserBuffer& m_buffer;
};
#endif
