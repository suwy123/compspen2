#if !defined(Z3BUFFER_)
#define Z3BUFFER_
/*******************************************
*  @file  Z3Buffer.h                       *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2019-2-17                     *
*                                          *
*******************************************/

#include "z3++.h"
#include "SortType.h"
#include <map>
//#include "Parser.h"

using namespace z3;
using std::map;


/**
 * the expr comparator
 */
class exprcomp {
public:
        bool operator() (const z3::expr& lhs, const z3::expr& rhs) const {
                return lhs.hash() < rhs.hash();
        }
};

using Z3ExprSet = set<expr, exprcomp>;
using ArgTypeList = vector<SortType*>;
using ParTypeList = vector<string>;
class FuncType;
class Parser;
class Var;

class Z3Buffer {
public:
	z3::context& z3_ctx;
    Z3Buffer(z3::context& ctx): z3_ctx(ctx) {}

    sort& getSort(SortType* pst); 

    void init(Parser& parser);

    void setVarEnv(Parser& parser);
    vector<expr>& getVarEnv() {return m_var_env;}

    void addVarEnv(expr var) {m_var_env.push_back(var);}

    expr& getVar(Var* pvar);

    func_decl getFuncDecl(FuncType* pft, ArgTypeList& arg_type_list, Parser& parser);
    func_decl getFuncDecl(string key);
    sort getSort(string key);

    expr getEmptyset();
    expr getSetinterset(expr& S1, expr& S2);
    expr getSetunion(expr& S1, expr& S2);
    expr getSet(expr i);
    expr getSetminus(expr& S1, expr& S2);
    expr getSubset(expr& S1, expr& S2);
    expr getBelongsto(expr& i, expr& S);
    expr getMax(expr& S);
    expr getMin(expr& S);

    void getQuantifierBounds(expr e, expr_vector& bounds, expr& body);

    void getIntItems(expr e, Z3ExprSet& items);

    expr getQuantElmt(z3::expr tm1, z3::expr tm2);

    void getBVars(expr e, Z3ExprSet& vars_set);
    void getFoVars(expr e, Z3ExprSet& vars_set);
    void getSoVars(expr e, Z3ExprSet& vars_set);
    void getLVars(expr e, Z3ExprSet& vars_set);

    bool isConstant(expr e);

    int indexOf(vector<expr>& vec, expr& e);
    int indexOf(vector<int>& vec, int i);

    expr mkEq(expr v1, expr v2);
    bool isFun(expr e, string s);
    bool isLocation(expr e);

    expr getFirstElement(int case_i, expr S);

    expr mkIntVar(string prefix, int p, int a, int q);
    expr mkIntVar(string prefix, int i);

    void show();

protected:
    map<string, sort> z3_sort_table;
    map<string, expr> z3_var_table;
    map<string, func_decl> z3_fun_table; 

    vector<expr> m_var_env;
};


#endif
