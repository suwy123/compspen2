/*******************************************
*  @file  Parser.cpp                       * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "Parser.h"
#include "Types.h"
#include "CommandParserFactory.h"
#include "component/Z3Buffer.h"


extern SyntaxErrorTable SYNTAX_ERROR_INFO;
extern Z3Buffer z3_buffer;
extern z3::context z3_ctx;

/*! @brief Brief function description here
 *
 *  Detailed description
 *
 * @return Return parameter description
 */
void Parser::parse() {
    // parse ( and symbol
    CommandParserFactory factory;
    CommandParser* cmd_parser;
    Token* curr = nullptr;
    while((curr=nextToken())!=nullptr
    && curr->type() == LEFT_PAREN) {
        curr = checkNext(SYMBOL_TOKEN, "command symbol is excepted!");
        cmd_parser = factory.getCommandParser(dynamic_cast<StrToken*>(curr)->value());
        // check
        if (cmd_parser == nullptr) {
            throw SemanticException("unsupported command!", curr->row(), curr->col());
        }
        
        cmd_parser->parse(*this);
    }
    if (curr != nullptr && curr->type() != EOF_TOKEN) {
        throw SemanticException("'(' is expected!", curr->row(), curr->col());
    }
}


void Parser::addSort(string key, SortType* value, int row, int col) {
    if (m_sort_table.find(key) != m_sort_table.end()) {
        throw SemanticException("Redefined sort!", row, col); 
    }
    m_sort_table[key] = value;
}

void Parser::addFunc(string key, FuncType* value, int row, int col) {
    if (m_func_table.find(key) != m_func_table.end()) {
        throw SemanticException("Redefined function!", row, col); 
    }
    m_func_table[key] = value;
}


void Parser::popVar() {
    int start = m_scope_mark_stack.back();
    m_scope_mark_stack.pop_back();
    for (auto pv=m_var_stack.begin()+start; pv != m_var_stack.end(); pv++) {
        delete (*pv);
    }
    m_var_stack.erase(m_var_stack.begin()+start, m_var_stack.end());
}

void Parser::topVar(VarList& vlist) {
    int start = m_scope_mark_stack.back();
    for (auto pv=m_var_stack.begin()+start; pv != m_var_stack.end(); pv++) {
       vlist.push_back(*pv); 
    }
}

void Parser::popArg() {
    m_arg_stack.pop_back();
}

void Parser::mkApp() {
    if (!m_op_stack.empty()) {
        string op = m_op_stack.back();

        // cout << "Before make " << op << endl;
        // showEnv();

        int arg_start = m_arg_scope_stack.back();
        ArgTypeList arg_type_list;

        for (auto pa = m_arg_stack.begin()+arg_start; pa != m_arg_stack.end(); pa++) {
            string sname = (*pa).get_sort().name().str();
            arg_type_list.push_back(m_sort_table.at(sname));
        }

        FuncType* pf = getFunc(op);
        if (pf != nullptr) {
            //make app
            func_decl fd = z3_buffer.getFuncDecl(pf, arg_type_list, *this);

            z3::expr_vector args(z3_ctx);
            for (auto pa = m_arg_stack.begin()+arg_start; pa != m_arg_stack.end(); pa++) {
                args.push_back(*pa);
            }
            
            expr res(z3_ctx);
            
			if(m_problem.getLogic()!="QF_SLID_SET"){
	            if(pf->getName()=="and"){
	            	res = mk_and(args);
				}else if(pf->getName()=="or"){
	            	res = mk_or(args);
				}else if(pf->getName()=="not"){
					if(args.size()==0){
						res = fd(args);
					}else
	            		res = !(args[0]);
				}else if(pf->getName()=="and"){
	            	res = mk_and(args);
				}else if(pf->getName()=="-"){
	            	res = args[0];
	            	if (args.size() == 1) {
	                        res = -args[0];
	                }
	            	for (unsigned i=1; i<args.size(); i++) {
	                    res = res - args[i];
	                }
				}else if(pf->getName()=="+"){
	            	res = sum(args);
				}else if(pf->getName()=="*"){
	            	res = args[0];
	            	for (unsigned i=1; i<args.size(); i++) {
	                    res = res * args[i];
	                }
				}else if(pf->getName()==">="){
	            	res = args[0] >= args[1];
				}else if(pf->getName()=="<="){
	            	res = args[0] <= args[1];
				}else if(pf->getName()==">"){
	            	res = args[0] > args[1];
				}else if(pf->getName()=="<"){
	            	res = args[0] < args[1];
				}else if(pf->getName()=="=" && args[0].to_string() != "nil"&&args[1].to_string() != "nil"&&args[0].get_sort().sort_kind() != Z3_UNINTERPRETED_SORT){
					res = args[0] == args[1];
				}else if(pf->getName()=="distinct"&& args[0].to_string() != "nil"&&args[1].to_string() != "nil"&&args[0].get_sort().sort_kind() != Z3_UNINTERPRETED_SORT){       	
					res = args[0] != args[1];
				}else{
					res = fd(args);
				}
			}else{
				res = fd(args);
			}
            
            
            // clear
            m_arg_stack.erase(m_arg_stack.begin() + arg_start, m_arg_stack.end());
            m_arg_stack.push_back(res);
            m_arg_scope_stack.pop_back();
            m_op_stack.pop_back();
        }

        // cout << "After make " << op << endl;
        // showEnv();
    }
}

void Parser::showEnv() {
    cout << "var environment: \n";
    int i = 0;
    for (auto item: m_var_stack) {
        cout << i++ << ": ";
        item->show();
        cout << endl;
    }
    cout << "var scope: \n";
    for (auto item : m_scope_mark_stack) {
        cout << item << " ";
    }
    cout << endl;
    cout << endl;

    cout << "expr environment: \n";
    cout << "op stack: \n";
    for (auto item: m_op_stack) {
        cout << item << endl;
    }
    cout <<endl;
    cout << "arg stack: \n";
    i = 0;
    for (auto item: m_arg_stack) {
        cout << i++ << ": ";
        cout << item;
        cout << endl;
    }
    for (auto item : m_arg_scope_stack) {
        cout << item << " ";
    }
    cout << endl;
    cout << endl;

}

void Parser::show() {
    cout << "supported sort: \n";
    for (auto item : m_sort_table) {
        cout << item.first << " ";
    }
    cout << endl;
    cout << "supported func: \n";
    for (auto item: m_func_table) {
        item.second->show();
        cout << endl;
    }
    cout << endl;

    showEnv();
}


Parser::Parser(istream& is, TokenScannerFactory& factory) :m_scanner(is), m_factory(factory) { 
    addVarScope();
}

Parser::~Parser() {
    for (auto item : m_sort_table) {
        delete item.second;
    }
    for (auto item : m_func_table) {
        delete item.second;
    }
}
