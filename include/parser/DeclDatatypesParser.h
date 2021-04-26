#if !defined(DECLDATATYPESPARSER_H)
#define DECLDATATYPESPARSER_H
/*******************************************
*  @file  DeclDatatypesParser.h            *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "CommandParser.h"


class DeclDatatypesParser: public CommandParser
{
public:
    DeclDatatypesParser() { }
    virtual ~DeclDatatypesParser() { }

    virtual void parse(Parser& parser);

};




#endif


