#if !defined(DECLCONSTPARSER_H)
#define DECLCONSTPARSER_H
/*******************************************
*  @file  DeclConstParser.h                *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "CommandParser.h"

class DeclConstParser: public CommandParser
{
public:
    DeclConstParser(/* args */) { }
    virtual ~DeclConstParser() { }
    virtual void parse(Parser& parser);
};

#endif