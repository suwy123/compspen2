#include "solver_slid_set/MonaTranslator.h"
#include "component/Z3Buffer.h"
#include <fstream>

extern z3::context z3_ctx;
extern Z3Buffer z3_buffer;

std::string mop_strs[] = {"UNDEF",
    " & ", " | ", "~", " => ", " <= ",
    " < ", " >= ", " > ", " = ", " ~= ", " sub ", " in ", "{$}",
    " + ", " - ", "min ", "max ", " union ",
    " inter ", " \\ "
};

MonaTranslator::MonaTranslator(z3::expr formula):  m_formula(formula){
    op_map["and"] = MONA_AND;
    op_map["or"] = MONA_OR;
    op_map["not"] = MONA_NOT;
    op_map["=>"] = MONA_IMPLY;
    op_map["<="] = MONA_LE;
    op_map["<"] = MONA_LT;
    op_map[">="] = MONA_GE;
    op_map[">"] = MONA_GT;
    op_map["="] = MONA_EQ;
    op_map["distinct"] = MONA_DISTINCT;
    op_map["+"] = MONA_PLUS;
    op_map["-"] = MONA_SUBSTRACT;
    //     op_map["*"] = MONA_MUL;
    // op_map["div"] = MONA_DIV;
    op_map["min"] = MONA_MIN;
    op_map["max"] = MONA_MAX;
    op_map["setunion"] = MONA_UNION;
    op_map["setintersect"] = MONA_INTER;
    op_map["setminus"] = MONA_MINUS;
    op_map["subset"] = MONA_SUB;
    op_map["belongsto"] = MONA_IN;
    op_map["set"] = MONA_SET;
}


/**
 * get string of formula
 */
void MonaTranslator::writeToFile(std::string name) {
    // std::cout << "write to file: " << name << std::endl;
    std::ofstream out(name);
    out << "ws1s;\n";
    std::set<z3::expr, exprcomp> bool_vars;
    std::set<z3::expr, exprcomp> fo_vars;
    std::set<z3::expr, exprcomp> so_vars;

    z3_buffer.getBVars(m_formula, bool_vars);
    z3_buffer.getFoVars(m_formula, fo_vars);
    z3_buffer.getSoVars(m_formula, so_vars);


    /*vector<expr>& var_env = z3_buffer.getVarEnv();
    for (auto v : var_env) {
        if (v.get_sort().to_string() == "Bool") {
            bool_vars.insert(v);
        } else if (v.get_sort().to_string() == "SetInt") {
            so_vars.insert(v);
        } else {
            fo_vars.insert(v);
        }
    }
    */

    std::string bool_decl_str = getDeclStr(0, bool_vars);

    std::string fo_decl_str = getDeclStr(1, fo_vars);

    std::string so_decl_str = getDeclStr(2, so_vars);


    if (bool_decl_str != "")
            out <<"var" << bool_decl_str << ";" << std::endl;
    if (fo_decl_str != "")
            out <<"var" <<fo_decl_str << ";" << std::endl;
    if (so_decl_str != "")
            out<<"var" << so_decl_str << ";" << std::endl;

    for (auto so_var : so_vars) {
            std::string var_name = so_var.to_string();
            if (var_name.find("minus") != -1) {
                    out << "0 notin " << var_name << ";\n";
            }
    }

    // 2. formula
    std::set<std::string> set_items;
    std::string formula_str = getStr(m_formula, set_items);

    out << formula_str << ";" << std::endl;

    out.close();
}

/**
 * get string of item
 * @param item
 * @param set_items : nonempty set items
 * @return string
 */
std::string MonaTranslator::getStr(z3::expr item, std::set<std::string>& set_items) {
    std::string str = "(";
    if (item.is_app()) {
        int num_args = item.num_args();
        if (num_args == 0) {
            if (item.decl().name().str() == "emptyset") {
                return "empty";
            }
            return item.to_string(); 
        }
        // operation
        std::string op = item.decl().name().str();
        mona_op mop = op_map[op];
        if (mop == 0) {
            std::cout << "UNSUPPORTED MONA OP: " << op << " !\n";
            exit(-1);
        }
        std::string op_str = mop_strs[mop];
        if (mop <= MONA_OR) {
            int size = item.num_args();
            for (int i=0; i<size; i++) {
                str.append(getStr(item.arg(i), set_items));
                if (i<size-1) str.append(op_str);
            }
        } else if(num_args == 2) {
            //
            std::set<std::string> set_items1;
            std::set<std::string> set_items2;
            std::string item1 = getStr(item.arg(0), set_items1);
            std::string item2 = getStr(item.arg(1), set_items2);


            for (auto tmp : set_items2) {
                set_items1.insert(tmp);
            }
            for (auto tmp : set_items1) {
                set_items.insert(tmp);
            }


            str.append(item1);
            str.append(op_str);
            str.append(item2);
            if (mop >= MONA_LE && mop <= MONA_IN) {
                for (auto tmp : set_items1) {
                    str.append(" & empty ~= ").append(tmp);
                }
            }
        } else if (num_args == 1) {
            std::string item1 = getStr(item.arg(0), set_items);

            if (mop == MONA_MIN || mop == MONA_MAX) {
                set_items.insert(item1);
            }

            if (mop == MONA_SET) {
                str.append("{").append(item1).append("}");
            } else {
                str.append(op_str);
                str.append(item1);
            }

        }
    } else if (item.is_quantifier()){
        // quantifiers
        z3::expr_vector bounds(z3_ctx);
        z3::expr body(z3_ctx);
        z3_buffer.getQuantifierBounds(item, bounds, body);
        bool is_forall = Z3_is_quantifier_forall(Z3_context(z3_ctx), Z3_ast(item));
        std::string bounds_str = getQuantifierBoundStr(is_forall, bounds);

        std::string body_str = getStr(body, set_items);

        str.append(bounds_str).append("\n").append(body_str);
    }
    str.append(")");
    return str;
}

/**
 * get string of quantifier bounds
 * @param is_all : forall flag
 * @param bounds :
 * @return string
 */
std::string MonaTranslator::getQuantifierBoundStr(bool is_all, z3::expr_vector &bounds) {
    std::set<z3::expr, exprcomp> bool_vars;
    std::set<z3::expr, exprcomp> fo_vars;
    std::set<z3::expr, exprcomp> so_vars;
    for (unsigned int i=0; i<bounds.size(); i++) {
    if (bounds[i].is_bool()) {
        bool_vars.insert(bounds[i]);
    } else if (bounds[i].is_int()) {
        fo_vars.insert(bounds[i]);
    } else {
        so_vars.insert(bounds[i]);
    }
    }
    std::string str="";
    std::string quant_str = is_all? "all" : "ex";
    std::string str0 = getDeclStr(0, bool_vars);
    std::string str1 = getDeclStr(1, fo_vars);
    std::string str2 = getDeclStr(2, so_vars);

    if (str0 != "") str.append(quant_str).append(str0).append(": ");
    if (str1 != "") str.append(quant_str).append(str1).append(": ");
    if (str2 != "") str.append(quant_str).append(str2).append(": ");

    return str;
}

/**
 * get string of declare vars
 * @param order : 0, 1, 2
 * @param var_set : vars
 * @return string
 */
std::string MonaTranslator::getDeclStr(int order, std::set<z3::expr, exprcomp> &var_set) {
    if (order > 2 || var_set.size() == 0) return "";


    std::string str = "";
    if (order == 0) {
        str.append("0 ");
    } else if (order == 1) {
        str.append("1 ");
    } else if (order == 2) {
        str.append("2 ");
    }

    int size = var_set.size();
    int i=0;
    for (auto exp : var_set) {
        str.append(exp.to_string());
        if (i<size-1) str.append(", ");
        i++;
    }
    // str.append(";\n");
    return str;
}
