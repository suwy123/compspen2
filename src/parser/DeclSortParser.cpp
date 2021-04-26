/*******************************************
*  @file  DeclSortParser.cpp               *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "parser/DeclSortParser.h"
#include "component/SortType.h"
#include "component/Z3Buffer.h"

extern SyntaxErrorTable SYNTAX_ERROR_INFO;
extern Z3Buffer z3_buffer;

/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @param parser Parameter description
 * @return Return parameter description
 */
void DeclSortParser::parse(Parser& parser) {
    Token* curr = parser.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
    string sort = dynamic_cast<StrToken*>(curr)->value();
    int row = curr->row();
    int col = curr->col();

    curr = parser.checkNext(INT_TOKEN, SYNTAX_ERROR_INFO[INT_TOKEN]);
    int snum = dynamic_cast<IntToken*>(curr)->value();

    curr = parser.checkNext(RIGHT_PAREN, SYNTAX_ERROR_INFO[RIGHT_PAREN]);

    SortType* p_sort = new SortType(sort, snum);
    parser.addSort(sort, p_sort, row, col);
    z3_buffer.getSort(p_sort);

	string nil = "nil";
	if(parser.getVar(nil) == nullptr){
		Var* pv = new Var(nil, p_sort);
    	parser.addVar(pv);	
	}
	string setint = "SetInt";
	string emptyset = "emptyset";
    if(parser.getSort(setint) != nullptr && parser.getVar(emptyset) == nullptr){
    	SortType* ps = parser.getSort(setint);
    	Var* pv = new Var(emptyset, ps);
    	parser.addVar(pv);	
	}
cout<<"declare-sort done"<<endl;
}

