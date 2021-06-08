#if !defined(PARSER_)
#define PARSER_
/*******************************************
*  @file  Parser.h                         * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include <iostream>
#include "z3++.h"
#include "Types.h"
#include "Scanner.h"
#include "TokenScannerFactory.h"
#include "exception/SemanticException.h"
#include "exception/SyntaxException.h"
#include "component/FuncType.h"
#include "component/SortType.h"
#include "component/Var.h"
#include "component/Problem.h"


using SortTable = map<string, SortType*>; 
using FuncTable = map<string, FuncType*>;
using VarStack = vector<Var*>;
using ScopeMarkStack = vector<int>;
using OpStack = vector<string>;
using ArgStack = vector<expr>;
using ArgScopeStack = vector<int>; 

using VarList = vector<Var*>;

class Z3Buffer;

/*! @class Parser
 *  @brief Brief class description
 *
 *  Detailed description
 */
class Parser
{
public:
	z3::context& z3_ctx;
	Z3Buffer& z3_buffer;
    Parser(istream& is, TokenScannerFactory& factory, z3::context& ctx, Z3Buffer& buffer);
    virtual ~Parser();
    void parse();
    void skip() {m_scanner.skip();}
    Scanner& getScanner() {return m_scanner;}
    TokenScannerFactory& getFactory() {return m_factory;}

    Token* checkNext(TOKEN type, string info) { return m_scanner.checkNext(type, info);}
    Token* nextToken() {return m_scanner.nextToken();}

    void addSort(string key, SortType* value, int row=-1, int col=-1); 
    void addFunc(string key, FuncType* value, int row=-1, int col=-1); 

    SortType* getSort(string& key) {if (m_sort_table.find(key) != m_sort_table.end()) return m_sort_table[key]; return nullptr;}
    FuncType* getFunc(string& key) {if (m_func_table.find(key) != m_func_table.end()) return m_func_table[key]; return nullptr;}

    void addVar(Var* pvar) {m_var_stack.push_back(pvar);}
    void addVarScope() { m_scope_mark_stack.push_back(m_var_stack.size()); }
    void popVar();
    void topVar(VarList& vlist);

    Var* getVar(string& name) {for(auto pv=m_var_stack.rbegin(); pv != m_var_stack.rend(); pv++) {if ((*pv)->getName() == name) return *pv;} return nullptr; }

    void pushOp(string op) {m_op_stack.push_back(op); pushArgScope();}
    void pushArg(expr arg) {m_arg_stack.push_back(arg);}
    void popArg();
    expr topArg() {return m_arg_stack.back();}
    void pushArgScope() {m_arg_scope_stack.push_back(m_arg_stack.size());}

    // fill problem
    void addLogic(string logic) {m_problem.setLogic(logic);}
    void addPredicate(Predicate* pred) {m_problem.setPredicate(pred);} 
    void addHeapChunk(HeapChunk* hck) {m_problem.setHeapChunk(hck);} 
    void addPhi(expr& phi) {m_problem.setPhi(phi);}
    void addPsi(expr& psi) {m_problem.setPsi(psi);}
    Problem* getProblem() {return &m_problem;}

    void mkApp();

    void show();

    void showEnv();

    void showProblem() {m_problem.show();}


protected:
    Scanner m_scanner; ///< scanner
    TokenScannerFactory& m_factory; ///< factory
    SortTable m_sort_table; ///< global sort table
    FuncTable m_func_table; ///< global function table

    VarStack m_var_stack; ///< var environment
    ScopeMarkStack m_scope_mark_stack; ///< scope mark

    OpStack m_op_stack; ///< opration stack
    ArgStack m_arg_stack; ///< argument stack
    ArgScopeStack m_arg_scope_stack; ///< argument scope stack

    Problem m_problem; ///< problem description
    friend class Z3Buffer;
};

#endif
