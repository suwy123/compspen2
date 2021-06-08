#if !defined(SET_LOGIC_PARSER_)
#define SET_LOGIC_PARSER_
/*******************************************
*  @file  SetLogicParser.h                 * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "CommandParser.h"

/*! @class SetLogicParser
 *  @brief Brief class description
 *
 *  Detailed description
 */
class SetLogicParser : public CommandParser
{
public:
    SetLogicParser(z3::context& ctx, Z3Buffer& buffer): CommandParser(ctx, buffer){}
    virtual ~SetLogicParser() {}
    virtual void parse(Parser& parser);
};

#endif
