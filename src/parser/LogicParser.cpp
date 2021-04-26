/*******************************************
*  @file  LogicParser.cpp                  *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "Types.h"
#include "parser/LogicParser.h"

#include <fstream>
#include <iostream>

using std::ifstream;
using std::cout;
using std::endl;

extern SyntaxErrorTable SYNTAX_ERROR_INFO;

void LogicParser::parse(Parser& parser) {
    ifstream fin;

    string abs_file_path = m_prefix + m_logic + ".smt2";

    fin.open(abs_file_path, std::ios_base::binary);

    if (!fin.is_open()) {
        cout << "Cannot find LOGIC FILE: " << (m_prefix + m_logic + ".smt2") << endl;
        exit(-1);
    }
    Scanner scanner(fin); 

    Token* curr = nullptr;
    // ( logic
    curr = scanner.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);
    
    curr = scanner.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
    if (dynamic_cast<StrToken*>(curr)->value() != "logic")
        throw SyntaxException("'logic' command is expected!", curr->row(), curr->col());
    // logic
    curr = scanner.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
    if (dynamic_cast<StrToken*>(curr)->value() != m_logic)
        throw SemanticException(m_logic + " is expected!", curr->row(), curr->col());

    // :keyword info 
    while ( (curr = scanner.nextToken()) != nullptr 
        && curr->type() == KEYWORD_TOKEN) {
        string key = dynamic_cast<StrToken*>(curr)->value();
        if (key == "theories") {
            // :theories ()
            scanner.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);
            while (  (curr = scanner.nextToken()) != nullptr 
                && curr->type() == SYMBOL_TOKEN) {
                // read theory
                string theory = dynamic_cast<StrToken*>(curr)->value();
                m_theories.push_back(theory);
            }
            if (curr == nullptr || curr->type() != RIGHT_PAREN) {
                throw SyntaxException(SYNTAX_ERROR_INFO[SYMBOL_TOKEN] + " or " + SYNTAX_ERROR_INFO[RIGHT_PAREN], 
                    curr->row(), curr->col());
            }
        } else {
            // description info :key "description"
            string key = dynamic_cast<StrToken*>(curr)->value();
            curr = scanner.nextToken();
            if (curr->type() == STRING_TOKEN) {
                m_info[key] = dynamic_cast<StrToken*>(curr)->value();
            } else if (curr->type() == INT_TOKEN) {
                m_info[key] = to_string(dynamic_cast<IntToken*>(curr)->value());
            } else if (curr->type() == FLOAT_TOKEN) {
                m_info[key] = to_string(dynamic_cast<FloatToken*>(curr)->value());
            } else {
                string info = "description info is expected!";
                throw SyntaxException(info, curr->row(), curr->col());
            }
        }
    }
    if (curr == nullptr && curr->type() != RIGHT_PAREN) {
        throw SyntaxException(SYNTAX_ERROR_INFO[KEYWORD_TOKEN] + " or " + SYNTAX_ERROR_INFO[RIGHT_PAREN], 
            curr->row(), curr->col());
    }
}
