#if !defined(COMMAND_PARSER_)
#define COMMAND_PARSER_
/*******************************************
*  @file  CommandParser.h                  * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "Parser.h"

class Parser;

using SortTypeList = vector<SortType*>;
using SelectorDec = pair<string, SortType*>;
using SelectorDecList = vector<SelectorDec>;
using ConstructorDec = pair<string, SelectorDecList>;
using ConstructorDecList = vector<ConstructorDec>;

/*! @class CommandParser
 *  @brief Brief class description
 *
 *  Detailed description
 */
class CommandParser
{
public:
	z3::context& z3_ctx;
	Z3Buffer& z3_buffer;
    CommandParser(z3::context& ctx, Z3Buffer& buffer):m_paren_counter(0), z3_ctx(ctx), z3_buffer(buffer) {}
    virtual ~CommandParser() {}
    virtual void parse(Parser& parser) {}

protected:
    SortType* parseSortDecl(Parser& parser);
    void parseSortDeclList(Parser& parser, SortTypeList& st_list);
    void parseSelectorDecl(Parser& parser, SelectorDec& sel_dec);
    void parseSelectorDeclList(Parser& parser, SelectorDecList& sd_list);
    void parseConstructorDecl(Parser& parser, ConstructorDec& con_dec);
    void parseConstructorDeclList(Parser& parser, ConstructorDecList& cd_list);

    void parseParameters(Parser& parser);
    void parseExpr(Parser& parser);
    void parseExists(Parser& parser);
    SortType* parseSort(Parser& parser);

private:
    void _parseExpr(Parser& parser);
protected:
    int m_paren_counter;

};
#endif
