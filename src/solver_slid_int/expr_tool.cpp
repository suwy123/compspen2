#include "solver_slid_int/expr_tool.h"
#include <fstream>

void expr_tool::get_vars(z3::expr exp, std::set<z3::expr, exprcomp> &var_set) {
        if (exp.is_app()) {
                for (int i=0; i<exp.num_args(); i++) {
                        get_vars(exp.arg(i), var_set);
                }
        } else {
                if (exp.is_var()) {
                        var_set.insert(exp);
                }
        }
}

void expr_tool::get_lvars(z3::expr exp, std::set<z3::expr, exprcomp> &lvar_set) {
        if (exp.is_app()) {
                for (int i=0; i<exp.num_args(); i++) {
                        get_lvars(exp.arg(i), lvar_set);
                }
        } else {
                if (exp.is_var() && is_location(exp)) {
                       lvar_set.insert(exp);
                }
        }
}

void expr_tool::get_consts(z3::expr exp, std::set<z3::expr, exprcomp> &const_set) {
        if (exp.is_app()) {
                if (exp.is_const()  && !is_constant(exp) && !exp.is_array()) {
                        const_set.insert(exp);
                } else {
                        for (int i=0; i<exp.num_args(); i++) {
                                get_consts(exp.arg(i), const_set);
                        }
                }
        }
}

void expr_tool::get_lconsts(z3::expr exp, std::set<z3::expr, exprcomp> &lconst_set) {
        if (exp.is_app()) {
                if (exp.is_const() && !is_constant(exp) && !exp.is_array() && is_location(exp)) {
                        lconst_set.insert(exp);
                } else {
                        for (int i=0; i<exp.num_args(); i++) {
                                get_lconsts(exp.arg(i), lconst_set);
                        }
                }
        }
}

void expr_tool::get_dconsts(z3::expr exp, std::set<z3::expr, exprcomp> &dconst_set) {
        if (exp.is_app()) {
                if (exp.is_const() && !is_constant(exp) && !exp.is_array() && !is_location(exp)) {
                        dconst_set.insert(exp);
                } else {
                        for (int i=0; i<exp.num_args(); i++) {
                                get_dconsts(exp.arg(i), dconst_set);
                        }
                }
        }
}


void expr_tool::get_constants(z3::expr exp, std::set<z3::expr, exprcomp> &constants_set) {
        if (exp.is_app()) {
                if (exp.is_numeral()) {
                        constants_set.insert(exp);
                } if((exp.decl().name().str() == "to_real" || exp.decl().name().str() == "to_int")){
                        constants_set.insert(exp.arg(0));
                }else {
                        for (unsigned i=0; i<exp.num_args(); i++) {
                                get_constants(exp.arg(i), constants_set);
                        }
                }
        } else if(exp.is_quantifier()) {
                get_constants(exp.body(), constants_set);
        }
}





void expr_tool::get_all_field_of_pto(z3::expr pto, std::vector<z3::expr>& fields) {
        if (is_fun(pto, "pto")) {
                z3::expr sref = pto.arg(1);
                for (int i=0; i<sref.num_args(); i++) {
                    fields.push_back(sref.arg(i));
            	}
        }
}

void expr_tool::expr_set_to_vec(std::set<z3::expr, exprcomp> &expr_set, std::vector<z3::expr> &expr_vec) {
        for (auto e : expr_set) {
                expr_vec.push_back(e);
        }
}

bool expr_tool::is_sub_set(std::set<z3::expr, exprcomp> &expr_set1, std::set<z3::expr, exprcomp> &expr_set2) {
        for (auto e : expr_set1) {
                if (expr_set2.find(e) == expr_set2.end()) return false;
        }
        return true;
}

void expr_tool::diff_set(std::set<z3::expr, exprcomp> &expr_set1, std::set<z3::expr, exprcomp> &expr_set2, std::set<z3::expr, exprcomp> &expr_set3) {
        for (auto e : expr_set1) {
                if (expr_set2.find(e) == expr_set2.end()) expr_set3.insert(e);
        }
}

void expr_tool::union_set(std::set<z3::expr, exprcomp> &expr_set1, std::set<z3::expr, exprcomp> &expr_set2, std::set<z3::expr, exprcomp> &expr_set3) {
        for (auto e : expr_set1) {
               expr_set3.insert(e);
        }
        for (auto e : expr_set2) {
                expr_set3.insert(e);
        }
}

void expr_tool::inter_set(std::set<z3::expr, exprcomp> &expr_set1, std::set<z3::expr, exprcomp> &expr_set2, std::set<z3::expr, exprcomp> &expr_set3) {
        for (auto e : expr_set1) {
                if (expr_set2.find(e) != expr_set2.end()) expr_set3.insert(e);
        }
}

int expr_tool::index_of_exp(z3::expr exp, std::vector<z3::expr> &expr_vec) {
        for (int i=0; i<expr_vec.size(); i++) {
                if (exp.hash() == expr_vec[i].hash()) return i;
        }
        return -1;
}

int expr_tool::rindex_of_exp(z3::expr exp, std::vector<z3::expr> &expr_vec) {
        for (int i=expr_vec.size()-1; i>=0; i--) {
                if (exp.hash() == expr_vec[i].hash()) return i;
        }
        return -1;
}


bool expr_tool::is_constant(z3::expr exp) {
        if (exp.to_string() == "true" || exp.to_string()=="false" ||
                exp.to_string()=="emptyset" || exp.to_string()=="emp" || exp.to_string()=="nil" || exp.is_numeral()) return true;
        return false;
}

bool expr_tool::is_fun(z3::expr expr, std::string fname) {
        if (expr.is_app() && expr.decl().name().str() == fname) return true;
        return false;
}

bool expr_tool::is_location(z3::expr exp) {
        if (exp.to_string() == "nil") return true;
        if (exp.get_sort().sort_kind() == Z3_UNINTERPRETED_SORT && !is_setint(exp)) return true;
        return false;
}

bool expr_tool::is_setint(z3::expr exp) {
        if (exp.get_sort().to_string() == "SetInt") return true;
        return false;
}

bool expr_tool::is_setint_const(z3::expr exp) {
        if (exp.is_const() && exp.get_sort().to_string() == "SetInt") {
                return true;
        }
        return false;
}

bool expr_tool::is_int_const(z3::expr exp) {
        if (exp.is_const() && exp.get_sort().to_string() == "Int") {
                return true;
        }
        return false;
}

bool expr_tool::contain_setint(z3::expr exp) {
        if (exp.is_app()) {
                if (is_setint(exp)) return true;
                bool flag = false;
                for (int i=0; i<exp.num_args(); i++) {
                        flag = flag || contain_setint(exp.arg(i));
                }
                return flag;
        }
        return false;
}

z3::expr expr_tool::eq_exp(z3::context& ctx, z3::expr exp1, z3::expr exp2) {
        if (is_location(exp1)) {
                z3::expr exp1_int = ctx.int_const(exp1.to_string().c_str());
                z3::expr exp2_int = ctx.int_const(exp2.to_string().c_str());
                return exp1_int == exp2_int;
        } else {
                return exp1 == exp2;
        }
}

/**
 * make min(exp) or max(exp)
 * @param mm : 0 min,  1 max
 */
z3::expr expr_tool::mk_min_max(z3::context& ctx, int mm, z3::expr exp) {
        if(exp.get_sort().to_string() == "SetInt") {
                z3::sort set_int_s = ctx.uninterpreted_sort("SetInt");
                z3::sort int_s = ctx.int_sort();
                std::string name = "min";
                if (mm&1) name = "max";
                z3::func_decl f = ctx.function(name.c_str(), set_int_s, int_s);
                return f(exp);
        }
        return ctx.int_val(-1); // error
}

z3::expr expr_tool::mk_emptyset(z3::context &ctx) {
        z3::sort set_int_s = ctx.uninterpreted_sort("SetInt");
        return ctx.constant("emptyset", set_int_s);
}

z3::expr expr_tool::mk_set_var(z3::context &ctx, std::string name) {
        z3::sort set_int_s = ctx.uninterpreted_sort("SetInt");
        return ctx.constant(name.c_str(), set_int_s);
}

z3::expr expr_tool::mk_binary_set(z3::context &ctx, std::string name, z3::expr s1, z3::expr s2) {
        if (s1.get_sort().to_string() == "SetInt" && s2.get_sort().to_string() == "SetInt") {
                z3::sort set_int_s = ctx.uninterpreted_sort("SetInt");
                z3::func_decl f = ctx.function(name.c_str(), set_int_s, set_int_s, set_int_s);
                return f(s1, s2);
        }
        return ctx.bool_val(false);
}

z3::expr expr_tool::mk_binary_bool(z3::context &ctx, std::string name, z3::expr s1, z3::expr s2) {
        if (s1.get_sort().to_string() == "SetInt" && s2.get_sort().to_string() == "SetInt") {
                z3::sort set_int_s = ctx.uninterpreted_sort("SetInt");
                z3::sort bool_s = ctx.bool_sort();
                z3::func_decl f = ctx.function(name.c_str(), set_int_s, set_int_s, bool_s);
                return f(s1, s2);
        }
        return ctx.bool_val(false);
}

z3::expr expr_tool::mk_belongsto(z3::context &ctx, z3::expr x, z3::expr S) {
        if (x.get_sort().to_string() == "Int" && S.get_sort().to_string() == "SetInt") {
                z3::sort set_int_s = ctx.uninterpreted_sort("SetInt");
                z3::sort bool_s = ctx.bool_sort();
                z3::sort int_s = ctx.int_sort();
                z3::expr x_set = mk_single_set(ctx, x);
                z3::func_decl f = ctx.function("subset", set_int_s, set_int_s, bool_s);
                return f(x_set, S);
        }
        return ctx.bool_val(false);
}

z3::expr expr_tool::mk_single_set(z3::context &ctx, z3::expr x) {
        if (x.get_sort().to_string() == "Int") {
                z3::sort set_int_s = ctx.uninterpreted_sort("SetInt");
                z3::sort int_s = ctx.int_sort();
                z3::func_decl f = ctx.function("set", int_s, set_int_s);
                return f(x);
        }
        return mk_emptyset(ctx);
}

void expr_tool::get_zero_order_vars(z3::expr exp, std::set<z3::expr, exprcomp> &vars_set) {
        if (exp.is_app()) {
                if (exp.is_const() && !expr_tool::is_constant(exp)) {
                        if (exp.get_sort().to_string() == "Bool")
                                vars_set.insert(exp);
                } else {
                        for (int i=0; i<exp.num_args(); i++) {
                                get_zero_order_vars(exp.arg(i), vars_set);
                        }
                }
        } else if(exp.is_quantifier()) {
                get_zero_order_vars(exp.body(), vars_set);
        }
}

void expr_tool::get_first_order_vars(z3::expr exp, std::set<z3::expr, exprcomp> &vars_set) {
        if (exp.is_app()) {
                if (exp.is_const() && !expr_tool::is_constant(exp)) {
                        if (exp.get_sort().to_string() == "Int")
                                vars_set.insert(exp);
                } else {
                        for (int i=0; i<exp.num_args(); i++) {
                                get_first_order_vars(exp.arg(i), vars_set);
                        }
                }
        } else if(exp.is_quantifier()) {
                get_first_order_vars(exp.body(), vars_set);
        }
}

void expr_tool::get_second_order_vars(z3::expr exp, std::set<z3::expr, exprcomp> &vars_set) {
        if (exp.is_app()) {
                if (exp.is_const() && !expr_tool::is_constant(exp)) {
                        if (exp.get_sort().to_string() == "SetInt")
                                vars_set.insert(exp);
                } else {
                        for (int i=0; i<exp.num_args(); i++) {
                                get_second_order_vars(exp.arg(i), vars_set);
                        }
                }
        } else if(exp.is_quantifier()) {
                get_second_order_vars(exp.body(), vars_set);
        }
}


void expr_tool::get_pars_quantifier(z3::context &ctx, z3::expr exp, z3::expr_vector &bounds, z3::expr &body) {
        if (exp.is_quantifier()) {
                int bnum = Z3_get_quantifier_num_bound(Z3_context(ctx), Z3_ast(exp));
                for (int i=bnum-1; i>=0; i--) {
                        Z3_symbol sym = Z3_get_quantifier_bound_name(Z3_context(ctx), Z3_ast(exp), i);
                        Z3_sort sym_s = Z3_get_quantifier_bound_sort(Z3_context(ctx), Z3_ast(exp), i);
                        Z3_ast x = Z3_mk_const(Z3_context(ctx), sym, sym_s);
                        z3::expr bound(ctx, x);
                        bounds.push_back(bound);
                }
                body = exp.body().substitute(bounds);
        }
}

std::string expr_tool::get_mona_name(z3::expr exp) {
        if (exp.is_const() && exp.is_bool()) {
                std::string name = exp.to_string();
                if (name.find("|[") == 0) {
                        name = name.replace(name.find("|["), 2, "$");
                        name = name.replace(name.find("]|"), 2, "$");
                        name = name.replace(name.find(","), 1, "_");
                        return name;
                }
        }
        return exp.to_string();
}

z3::expr expr_tool::get_plus_exp(z3::context& ctx, z3::expr exp) {
        if (exp.is_const()) {
                std::string name = exp.to_string();
                name.append("_plus");
                return ctx.constant(name.c_str(), exp.get_sort());
        }
        return exp;
}

z3::expr expr_tool::get_minus_exp(z3::context& ctx, z3::expr exp) {
        if (exp.is_const()) {
                std::string name = exp.to_string();
                name.append("_minus");
                return ctx.constant(name.c_str(), exp.get_sort());
        }
        return exp;
}

z3::expr expr_tool::mk_item(z3::expr t_i_1, std::string R, z3::expr t_i_2, z3::expr c) {
        bool flag = false;
        if (t_i_2.is_numeral() && t_i_2.get_numeral_int() == 0) {
                flag = true;
        }
        if (R == "=") {
                return flag? t_i_1 == c : t_i_1 == (t_i_2 + c);
        } else if (R == "<=") {
                return flag? t_i_1 <= c : t_i_1 <= (t_i_2 + c);
        } else if (R == ">=") {
                return flag? t_i_1 >= c : t_i_1 >= (t_i_2 + c);
        }
        return t_i_1;
}

void expr_tool::get_singleset(z3::expr exp, std::set<z3::expr, exprcomp> &singles) {
        if (exp.is_app()) {
                if (is_fun(exp, "set")) {
                        singles.insert(exp);
                }
                for (int i=0; i<exp.num_args(); i++) {
                        get_singleset(exp.arg(i), singles);
                }
        }
}

bool expr_tool::get_singleset_min(z3::expr exp, z3::expr& S) {
        if (is_fun(exp, "set")) {
                z3::expr ele = exp.arg(0);
                if (is_fun(ele, "min") || is_fun(ele, "max")) {
                        S = ele.arg(0);
                        return true;
                }
        }
        return false;
}


void expr_tool::get_min_max_items(z3::expr exp, std::set<z3::expr, exprcomp> &items) {
        if (exp.is_app()) {
                if (is_fun(exp, "min") || is_fun(exp, "max")) {
                        if (!exp.arg(0).is_const()) {
                                items.insert(exp.arg(0));
                        }
                } else {
                        for (int i=0; i<exp.num_args(); i++) {
                                get_min_max_items(exp.arg(i), items);
                        }
                }
        }
}


z3::expr expr_tool::mk_bottom(z3::context &ctx) {
        return ctx.bool_const("bottom");
}

bool expr_tool::is_bottom(z3::expr exp) {
        return exp.get_sort().is_bool() && exp.to_string() == "bottom";
}

std::string expr_tool::get_pair(std::string pair1, std::string pair2) {
        std::string result = pair1;
        result.append(",").append(pair2);
        return result;
}

void expr_tool::write_file(std::string fname, z3::expr &formula) {
        std::ofstream out(fname);
        out << formula << std::endl;
        out.close();
}

/**
 * tm1 : T1 op T2 + c1
 * tm2 : T1 op T2 + c2
 * quantelmt
 */
z3::expr expr_tool::get_quant_elmt(z3::context& ctx, z3::expr tm1, z3::expr tm2) {
        // std::cout << "tm1: " << tm1 <<std::endl;
        // std::cout << "tm2: " << tm2 << std::endl;
        z3::expr result = ctx.bool_val(true);
        z3::expr tm1_1 = tm1.arg(0);
        z3::expr tm1_2 = tm1.arg(1).arg(0);
        int c1 = tm1.arg(1).arg(1).get_numeral_int();
        std::string op1 = tm1.decl().name().str();
        z3::expr tm2_1 = tm2.arg(0);
        z3::expr tm2_2 = tm2.arg(1).arg(0);
        int c2 = tm2.arg(1).arg(1).get_numeral_int();
        std::string op2 = tm2.decl().name().str();

        int c = gcd(c1, c2);
        c =  c1 * c2 / c; //
        c1 = c / c1;
        c2 = c / c2;
        if (c != 0) {
                int case_i = 0;
                if (op1 == "<=") case_i = 0;
                else if (op1 == "=") case_i = 1;
                else if (op1 == ">=") case_i = 2;

                if (op2 == "=") case_i += 3;
                else if(op2 == ">=") case_i += 6;

                // std::cout << "case_i: " << case_i << std::endl;

                switch(case_i) {
                case 0: // <= , <=
                case 8: // >=, >=
                        break;
                case 7: // =, >=
                case 6: // <=, >=
                case 3: // <=, =
                        result = c1 * (tm1_1 - tm1_2) <= c2 * (tm2_1 - tm2_2);
                        break;
                case 5: // >=, =
                case 2: // >=, <=
                case 1: // =, <=
                        result = c2 * (tm2_1 - tm2_2) <= c1 * (tm1_1 - tm1_2);
                        break;
                case 4:
                        result = c1 * (tm1_1 - tm1_2) == c2 * (tm2_1 - tm2_2);
                        break;
                default:
                        std::cout << "quant_elmt :: NOT SUPPOERED!\n";
                        exit(-1);
                }
        }
        return result;
}

int expr_tool::gcd(int a, int b) {
        return b == 0? a : gcd(b, a%b);
}

void expr_tool::get_items(z3::expr item, z3::expr_vector &items) {
        if (item.is_app()) {
                if (item.num_args() == 2) {
                        z3::expr item1 = item.arg(0);
                        z3::expr item2 = item.arg(1);
                        std::string op = item.decl().name().str();
                        if (item1.num_args() == 2) get_items(item1, items);
                        else items.push_back(item1);
                        if (item2.num_args() == 2) get_items(item1, items);
                        else {
                                if (op == "+") {
                                        items.push_back(item2);
                                } else {
                                        items.push_back(item2-1); // -
                                }
                        }
                }
        }
}


z3::expr expr_tool::mk_int_var(z3::context& ctx, std::string prefix, int i) {
        prefix.append(std::to_string(i));
        return ctx.int_const(prefix.c_str());
}

z3::expr expr_tool::mk_int_var(z3::context& ctx, std::string prefix, int p, int a, int q) {
        prefix.append(std::to_string(p)).append("_").append(std::to_string(a)).append("_").append(std::to_string(q));
        return ctx.int_const(prefix.c_str());
}
