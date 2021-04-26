#if !defined(DEFINE_FUNCTION_REC_PARSER_)
#define DEFINE_FUNCTION_REC_PARSER_
/*******************************************
*  @file  DefineFunctionRecParser.h        * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "CommandParser.h"

/*! @class DefineFunctionRecParser
 *  @brief Brief class description
 *
 *  Detailed description
 */
class DefineFunctionRecParser: public CommandParser
{
public:
    DefineFunctionRecParser() {}
    virtual ~DefineFunctionRecParser() {}
    virtual void parse(Parser& parser);
    void checkSLAHRecRule(Parser& parser, z3::expr rec, string fname);

};

#endif
