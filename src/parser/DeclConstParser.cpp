/*******************************************
*  @file  DeclConstParser.cpp              *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "parser/DeclConstParser.h"

extern SyntaxErrorTable SYNTAX_ERROR_INFO;

void DeclConstParser::parse(Parser& parser) {
    Token* curr = parser.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
    string vname = dynamic_cast<StrToken*>(curr)->value();
    
    curr = parser.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
    string vsort = dynamic_cast<StrToken*>(curr)->value();

    curr = parser.checkNext(RIGHT_PAREN, SYNTAX_ERROR_INFO[RIGHT_PAREN]);
     
    SortType* sort = parser.getSort(vsort);

    Var* pv = new Var(vname, sort);
    parser.addVar(pv);
cout<<"declare-const done"<<endl;
}
