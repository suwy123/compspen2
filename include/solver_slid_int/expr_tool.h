#ifndef EXPR_TOOL_H
#define EXPR_TOOL_H

#include <z3++.h>
#include <set>
#include <vector>
#include "component/Z3Buffer.h"


class expr_tool {
public:
        static void get_vars(z3::expr exp, std::set<z3::expr, exprcomp>& var_set);
        static void get_lvars(z3::expr exp, std::set<z3::expr, exprcomp>& lvar_set);
        static void get_consts(z3::expr exp, std::set<z3::expr, exprcomp>& const_set);
        static void get_lconsts(z3::expr exp, std::set<z3::expr, exprcomp>& lconst_set);
        static void get_dconsts(z3::expr exp, std::set<z3::expr, exprcomp>& dconst_set);
        static void get_constants(z3::expr exp,  std::set<z3::expr, exprcomp>& constants_set);

        static void expr_set_to_vec(std::set<z3::expr, exprcomp>& expr_set, std::vector<z3::expr>& expr_vec);
        static bool is_sub_set(std::set<z3::expr, exprcomp>& expr_set1, std::set<z3::expr, exprcomp>& expr_set2);
        static void union_set(std::set<z3::expr, exprcomp>& expr_set1, std::set<z3::expr, exprcomp>& expr_set2, std::set<z3::expr, exprcomp>& expr_set3);
        static void inter_set(std::set<z3::expr, exprcomp>& expr_set1, std::set<z3::expr, exprcomp>& expr_set2, std::set<z3::expr, exprcomp>& expr_set3);
        static void diff_set(std::set<z3::expr, exprcomp>& expr_set1, std::set<z3::expr, exprcomp>& expr_set2, std::set<z3::expr, exprcomp>& expr_set3);

        static void get_all_field_of_pto(z3::expr pto, std::vector<z3::expr>& fields);


        static int index_of_exp(z3::expr exp, std::vector<z3::expr>& expr_vec);
        static int rindex_of_exp(z3::expr exp, std::vector<z3::expr>& expr_vec);

        static bool is_constant(z3::expr exp);
        static bool is_fun(z3::expr expr, std::string fname);
        static bool is_location(z3::expr exp);
        static bool is_setint(z3::expr exp);
        static bool is_setint_const(z3::expr exp);
        static bool is_int_const(z3::expr exp);
        static bool contain_setint(z3::expr exp);

        static z3::expr eq_exp(z3::context& ctx, z3::expr exp1, z3::expr exp2);
        static z3::expr mk_min_max(z3::context& ctx, int mm, z3::expr exp);

        static z3::expr mk_emptyset(z3::context& ctx);
        static z3::expr mk_set_var(z3::context& ctx, std::string name);
        static z3::expr mk_binary_set(z3::context& ctx, std::string name, z3::expr s1, z3::expr s2);
        static z3::expr mk_binary_bool(z3::context& ctx, std::string name, z3::expr s1, z3::expr s2);

        static z3::expr mk_belongsto(z3::context& ctx, z3::expr x, z3::expr S);

        static z3::expr mk_single_set(z3::context& ctx, z3::expr x);

        static void get_zero_order_vars(z3::expr exp, std::set<z3::expr, exprcomp> &vars_set);
        static void get_first_order_vars(z3::expr exp, std::set<z3::expr, exprcomp> &vars_set);
        static void get_second_order_vars(z3::expr exp, std::set<z3::expr, exprcomp> &vars_set);

        static void get_pars_quantifier(z3::context& ctx, z3::expr exp, z3::expr_vector& bounds, z3::expr& body);

        static std::string get_mona_name(z3::expr exp);
        static z3::expr get_plus_exp(z3::context& ctx, z3::expr exp);
        static z3::expr get_minus_exp(z3::context& ctx, z3::expr exp);
        static z3::expr mk_item(z3::expr t_i_1, std::string R, z3::expr t_i_2, z3::expr c);


        static void write_file(std::string fname, z3::expr& formula);
        static void get_singleset(z3::expr exp, std::set<z3::expr, exprcomp>& singles);
        static bool get_singleset_min(z3::expr exp, z3::expr& S);
        static void get_min_max_items(z3::expr exp, std::set<z3::expr, exprcomp>& items);
        static z3::expr mk_bottom(z3::context& ctx);
        static bool is_bottom(z3::expr exp);

        static std::string get_pair(std::string pair1, std::string pair2);
        static z3::expr get_quant_elmt(z3::context& ctx, z3::expr tm1, z3::expr tm2);
        static int gcd(int a, int b);

        static void get_items(z3::expr item, z3::expr_vector& items);

        // used by fa.h
        static z3::expr mk_int_var(z3::context& ctx, std::string prefix, int i);
        static z3::expr mk_int_var(z3::context& ctx, std::string prefix, int p, int a, int q);


};



#endif /* EXPR_TOOL_H */
