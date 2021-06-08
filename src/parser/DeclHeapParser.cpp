/*******************************************
*  @file  DeclHeapParser.cpp               *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "parser/DeclHeapParser.h"

extern SyntaxErrorTable SYNTAX_ERROR_INFO;

void DeclHeapParser::parse(Parser& parser) {
    parser.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);

    SortType* sort_src = parseSort(parser);
    SortType* sort_dst = parseSort(parser);

    // cout << sort_src->getName() << "->" << sort_dst->getName() << endl;
    parser.getProblem()->setHeap(sort_src, sort_dst);

    parser.checkNext(RIGHT_PAREN, SYNTAX_ERROR_INFO[RIGHT_PAREN]);
    parser.checkNext(RIGHT_PAREN, SYNTAX_ERROR_INFO[RIGHT_PAREN]);
//cout<<"declare-heap done"<<endl;
}
