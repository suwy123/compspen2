/*******************************************
*  @file  SetLogicParser.cpp               * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "parser/SetLogicParser.h"
#include "parser/LogicParser.h"
#include "parser/TheoryParser.h"
#include "component/Z3Buffer.h"
#include <iostream>

extern SyntaxErrorTable SYNTAX_ERROR_INFO;
extern Z3Buffer z3_buffer;

using std::cout;
using std::endl;


/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @param parser Parameter description
 * @return Return parameter description
 */
void SetLogicParser::parse(Parser& parser) {
    Token* curr = parser.nextToken();
    // Token* curr = parser.checkNext(STRING_TOKEN, SYNTAX_ERROR_INFO[STRING_TOKEN]);
    if (curr->type() != STRING_TOKEN && curr->type() != SYMBOL_TOKEN) {
        throw SemanticException("logic name is expected!", curr->row(), curr->col());
    }
    //
    string logic = dynamic_cast<StrToken*>(curr)->value();
	parser.addLogic(logic); 

    // cout << "logic: " <<  logic << endl;
    LogicParser logic_parser(logic);
    logic_parser.parse(parser);
    Theories& theories = logic_parser.getTheories();
    for (string theory : theories) {
        // cout << "theory: " << theory << endl;
        TheoryParser theory_parser(theory);
        theory_parser.parse(parser);
    }

    curr = parser.checkNext(RIGHT_PAREN, SYNTAX_ERROR_INFO[RIGHT_PAREN]); 
    // action: init context by logic file
    z3_buffer.init(parser);
cout<<"set-logic done"<<endl;
}
