#if !defined(ASSERT_PARSER_)
#define ASSERT_PARSER_

/*******************************************
*  @file  AssertParser.h                   * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "CommandParser.h"

/*! @class AssertParser
 *  @brief Brief class description
 *
 *  Detailed description
 */
class AssertParser: public CommandParser
{
public:
    AssertParser() {}
    virtual ~AssertParser() {}
    virtual void parse(Parser& parser);

};

#endif
