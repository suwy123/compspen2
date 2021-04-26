#if !defined(DECL_SORT_PARSER_)
#define DECL_SORT_PARSER_
/*******************************************
*  @file  DeclSortParser.h                 * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "CommandParser.h"

/*! @class DeclSortParser
 *  @brief Brief class description
 *
 *  Detailed description
 */
class DeclSortParser: public CommandParser
{
public:
    DeclSortParser() {}
    virtual ~DeclSortParser() {}
    virtual void parse(Parser& parser);

};

#endif
