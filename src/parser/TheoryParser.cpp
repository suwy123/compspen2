/*******************************************
*  @file  TheoryParser.cpp                 *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "parser/TheoryParser.h"
#include "Types.h"


#include <fstream>
#include <iostream>

using std::ifstream;
using std::cout;
using std::endl;

extern SyntaxErrorTable SYNTAX_ERROR_INFO;

void TheoryParser::parse(Parser& parser) {
    ifstream fin;

    string abs_file_path = m_prefix + m_theory + ".smt2";
    cout << "THEORY path: " << abs_file_path << endl;

    fin.open(abs_file_path, std::ios_base::binary);

    if (!fin.is_open()) {
        cout << "Cannot find THEORY FILE: " << (m_prefix + m_theory + ".smt2") << endl;
        exit(-1);
    }
    Scanner scanner(fin); 

    Token* curr = nullptr;
    // (theory 
    curr = scanner.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);
    
    curr = scanner.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
    if (dynamic_cast<StrToken*>(curr)->value() != "theory")
        throw SyntaxException("'theory' command is expected!", curr->row(), curr->col());
    // logic
    curr = scanner.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
    if (dynamic_cast<StrToken*>(curr)->value() != m_theory)
        throw SemanticException(m_theory + " is expected!", curr->row(), curr->col());

    // :keyword info 
    while ( (curr = scanner.nextToken()) != nullptr 
        && curr->type() == KEYWORD_TOKEN) {
        string key = dynamic_cast<StrToken*>(curr)->value();
        if (key == "sorts") {
            // parse sorts
            // ( () () )
            curr = scanner.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);
            while ((curr = scanner.nextToken()) != nullptr 
                && curr->type() == LEFT_PAREN) {
                curr = scanner.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
                string sort = dynamic_cast<StrToken*>(curr)->value();
                curr = scanner.checkNext(INT_TOKEN, SYNTAX_ERROR_INFO[INT_TOKEN]);
                int snum = dynamic_cast<IntToken*>(curr)->value();
                scanner.checkNext(RIGHT_PAREN, SYNTAX_ERROR_INFO[RIGHT_PAREN]);
                // cout << "SORT: " << sort << ", " << snum << endl;
                SortType* p_sort = new SortType(sort, snum);
                parser.addSort(sort, p_sort);
            }
            if (curr == nullptr || curr->type() != RIGHT_PAREN) {
                throw SyntaxException(SYNTAX_ERROR_INFO[RIGHT_PAREN], curr->row(), curr->col());
            }
        } else if (key == "funs") {
            // parse funs
            // ( () ())
            curr = scanner.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);
            while ((curr = scanner.nextToken()) != nullptr 
                && curr->type() == LEFT_PAREN) {
                curr = scanner.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
                string fun_name = dynamic_cast<StrToken*>(curr)->value();
                bool par_flag = false;
                
                vector<string> par_list;
                if (fun_name == "par") {
                    par_list.clear();
                    scanner.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);
                    par_flag = true;
                    // read parameter list
                    while ((curr = scanner.nextToken()) != nullptr 
                        && curr->type() == SYMBOL_TOKEN) {
                        par_list.push_back(dynamic_cast<StrToken*>(curr)->value());
                    }
                    if (curr == nullptr || curr->type() != RIGHT_PAREN) {
                        throw SyntaxException(SYNTAX_ERROR_INFO[RIGHT_PAREN], curr->row(), curr->col());
                    }
                    // read fun
                    scanner.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);
                    curr = scanner.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
                    fun_name = dynamic_cast<StrToken*>(curr)->value();
                }

                vector<string> sort_list;
                string attr = "NONE";
                while ( (curr = scanner.nextToken()) != nullptr 
                    && curr->type() == SYMBOL_TOKEN) {
                    sort_list.push_back(dynamic_cast<StrToken*>(curr)->value());
                }
                if (curr != nullptr && curr->type() == KEYWORD_TOKEN) {
                    attr = dynamic_cast<StrToken*>(curr)->value(); 
                    curr = scanner.nextToken();
                }
                if (curr == nullptr || curr->type() != RIGHT_PAREN) {
                    throw SyntaxException(SYNTAX_ERROR_INFO[RIGHT_PAREN], curr->row(), curr->col());
                }

                if (par_flag) {
                    scanner.checkNext(RIGHT_PAREN, SYNTAX_ERROR_INFO[RIGHT_PAREN]);
                }

                FuncType* p_fun = nullptr; 
                if (par_flag) {
                    p_fun = new ParFuncType(fun_name);
                } else {
                    if (fun_name == "-" && attr == "NONE") {
                        fun_name = "--"; // negative
                    }
                    p_fun = new FuncType(fun_name);
                }

                for (string par: par_list) {
                    p_fun->addPar(par);
                }
                for (string sort : sort_list) {
                    p_fun->addArg(sort);
                }
                p_fun->setAttr(attr);
                parser.addFunc(fun_name, p_fun);
            }
            if (curr == nullptr || curr->type() != RIGHT_PAREN) {
                throw SyntaxException(SYNTAX_ERROR_INFO[RIGHT_PAREN], curr->row(), curr->col());
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

}