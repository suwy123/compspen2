#if !defined(DEFINE_FUNCTION_PARSER_)
#define DEFINE_FUNCTION_PARSER_
/*******************************************
*  @file  DefineFunctionParser.h           * 
*  @brief    Brief file description        *
*                                          *
*  @author   Wanyun Su                     *
*  @version  1.0                           *
*  @date     2021-4-8                      *
*                                          *
*******************************************/

#include "CommandParser.h"

/*! @class DefineFunctionRecParser
 *  @brief Brief class description
 *
 *  Detailed description
 */
class DefineFunctionParser: public CommandParser
{
public:
    DefineFunctionParser(z3::context& ctx, Z3Buffer& buffer): CommandParser(ctx, buffer) {}
    virtual ~DefineFunctionParser() {}
    virtual void parse(Parser& parser);

};

#endif
