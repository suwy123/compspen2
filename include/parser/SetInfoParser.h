#if !defined(SET_INFO_PARSER_)
#define SET_INFO_PARSER_
/*******************************************
*  @file  SetInfoParser.h                  * 
*  @brief    Brief file description        *
*                                          *
*  @author   Su WanYun                     *
*  @version  1.0                           *
*  @date     2021-3-1                     *
*                                          *
*******************************************/

#include "CommandParser.h"

/*! @class SetLogicParser
 *  @brief Brief class description
 *
 *  Detailed description
 */
class SetInfoParser : public CommandParser
{
public:
    SetInfoParser() {}
    virtual ~SetInfoParser() {}
    virtual void parse(Parser& parser);
};

#endif
