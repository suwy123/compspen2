/*******************************************
*  @file  DeclDatatypesParser.cpp          *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "parser/DeclDatatypesParser.h"

//#include "component/Z3Buffer.h"

extern SyntaxErrorTable SYNTAX_ERROR_INFO;
//extern Z3Buffer z3_buffer;

void DeclDatatypesParser::parse(Parser& parser) {
    parser.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);
    SortTypeList st_list;
    parseSortDeclList(parser, st_list);
    // parse datatype
    parser.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);
    parser.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);
    ConstructorDecList cd_list;
    parseConstructorDeclList(parser, cd_list);

    parser.checkNext(RIGHT_PAREN, SYNTAX_ERROR_INFO[RIGHT_PAREN]);

    // action
    // 1. add sort (st_list.size() == 1)
    // assert(st_list.size() == 1);
    SortType* ptr = st_list[0];
    parser.addSort(ptr->getName(), ptr);
    // add z3 buffer
    z3_buffer.getSort(ptr);
    // 2. add func_decl (cd_list.size() == 1)
    ConstructorDec cd = cd_list[0];
    FuncType* pf = new FuncType(z3_ctx,z3_buffer,cd.first);
    for (auto item : cd.second) {
        pf->addArg(item.second->getName());
    }
    pf->addArg(ptr->getName());
    parser.addFunc(cd.first, pf);
//cout<<"declare-datatypes done"<<endl;
}
