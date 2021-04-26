#if !defined(COMMAND_PARSER_BUFFER_)
#define COMMAND_PARSER_BUFFER_
/*******************************************
*  @file  CommandParserBuffer.h            * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include <map>

#include "CommandParser.h"
#include "parser/SetLogicParser.h"
#include "parser/SetInfoParser.h"
#include "parser/DeclSortParser.h"
#include "parser/DeclDatatypesParser.h"
#include "parser/DeclHeapParser.h"
#include "parser/DefineFunctionParser.h"
#include "parser/DefineFunctionRecParser.h"
#include "parser/DeclConstParser.h"
#include "parser/AssertParser.h"
#include "parser/CheckSatParser.h"

using std::map;
using std::string;

class CommandParser;

/*! @class CommandParserBuffer
 *  @brief Brief class description
 *
 *  Detailed description
 */
class CommandParserBuffer
{
public:
    CommandParserBuffer(); 
    virtual ~CommandParserBuffer(); 

    CommandParser* getCommandParser();

    DeclHeapParser* getDeclHeapParser();

    DeclSortParser* getDeclSortParser();

    DeclDatatypesParser* getDeclDatatypesParser();

    SetLogicParser* getSetLogicParser();
    
    SetInfoParser* getSetInfoParser();
    
    DefineFunctionParser* getDefineFunctionParser();

    DefineFunctionRecParser* getDefineFunctionRecParser();

    DeclConstParser* getDeclConstParser();

    AssertParser* getAssertParser();

    CheckSatParser* getCheckSatParser();

protected:
    map<string, CommandParser*> m_buffer; ///< Member description
};


#endif
