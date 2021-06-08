/*******************************************
*  @file  SetInfoParser.cpp                * 
*  @brief    Brief file description        *
*                                          *
*  @author   Su WanYun                     *
*  @version  1.0                           *
*  @date     2021-3-1                     *
*                                          *
*******************************************/

#include "parser/SetInfoParser.h"
#include "component/Z3Buffer.h"
#include <iostream>

extern SyntaxErrorTable SYNTAX_ERROR_INFO;
//extern Z3Buffer z3_buffer;

using std::cout;
using std::endl;


/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @param parser Parameter description
 * @return Return parameter description
 */
void SetInfoParser::parse(Parser& parser) {
    Token* curr = parser.nextToken();
    // Token* curr = parser.checkNext(STRING_TOKEN, SYNTAX_ERROR_INFO[STRING_TOKEN]);
    while (curr!=nullptr && curr->type() != RIGHT_PAREN) {
    	curr=parser.nextToken();
        //curr = parser.checkNext(STRING_TOKEN, SYNTAX_ERROR_INFO[STRING_TOKEN]);
    }

    //curr = parser.checkNext(RIGHT_PAREN, SYNTAX_ERROR_INFO[RIGHT_PAREN]); 

//cout<<"set-info done"<<endl;
}
