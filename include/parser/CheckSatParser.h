#if !defined(CHECKSATPARSER_)
#define CHECKSATPARSE_R
/*******************************************
*  @file  CheckSatParser.h                 *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2019-2-18                     *
*                                          *
*******************************************/
#include "CommandParser.h"

class CheckSatParser : public CommandParser {
public:
    CheckSatParser(z3::context& ctx, Z3Buffer& buffer): CommandParser(ctx, buffer) {}
    virtual ~CheckSatParser() {}

    virtual void parse(Parser& parser);
};
#endif
