#if !defined(DECLHEAPPARSER_H)
#define DECLHEAPPARSER_H
/*******************************************
*  @file  DeclHeapParser.h                 *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "CommandParser.h"

class DeclHeapParser: public CommandParser
{
public:
    DeclHeapParser(z3::context& ctx, Z3Buffer& buffer): CommandParser(ctx, buffer) { }
    virtual ~DeclHeapParser() { }
    virtual void parse(Parser& parser);
};

#endif
