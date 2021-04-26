/*******************************************
*  @file  FuncType.cpp                     *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "component/FuncType.h"
#include "component/Z3Buffer.h"
#include "exception/SemanticException.h"

extern z3::context z3_ctx;
extern Z3Buffer z3_buffer;

z3::func_decl FuncType::determine(ArgTypeList& arg_type_list){
    // Todo
    string fname = m_name;
    if (fname == "=>") {
        fname = "implies";
    }
    bool is_valid = true;
    string range;
    ArgTypeList arg_list;
    if (arg_type_list.size() == m_arg_list.size() - 1) {
        for (unsigned int i=0; i<arg_type_list.size(); i++) {
            if (arg_type_list[i]->getName() != m_arg_list[i]) {
                is_valid = false;
                break;
            }
        }
        range = m_arg_list.back();
        arg_list = arg_type_list;
    } else if (m_attr == "left-assoc") {
        for (unsigned int i=0; i<arg_type_list.size(); i++) {
            if (arg_type_list[i]->getName() != m_arg_list[0]) {
                is_valid = false;
                break;
            }
        }
        range = m_arg_list.back();
        arg_list = arg_type_list;
    }

    if (!is_valid) {
        throw SemanticException("the argument of " + fname + " is not matched!");
    }

    // mk
    z3::sort_vector svec(z3_ctx);
    for (auto arg : arg_list) {
        svec.push_back(z3_buffer.getSort(arg));
    }
    z3::sort srange = z3_buffer.getSort(range);
    return z3_ctx.function(fname.c_str(), svec, srange);
}

void FuncType::show() {
    cout << "func: " << m_name << " ";
    cout << " (";
    for (auto arg : m_arg_list) {
        cout << arg <<  " ";
    }
    cout << " <-ret) ";
    cout << "(attr: " << m_attr << ") ";
}

void ParFuncType::addArg(string arg) {
    m_arg_list.push_back(arg);
    if (m_par_set.find(arg) != m_par_set.end()) {
        m_par_arg_mark_list.push_back(true);
    } else {
        m_par_arg_mark_list.push_back(false);
    }
}

z3::func_decl ParFuncType::determine(ArgTypeList& arg_type_list) {
    string fname = m_name;
    bool is_valid = true;
    map<string, string> act_map;
    
    for (unsigned int i=0; i<arg_type_list.size(); i++) {
        if (m_par_arg_mark_list[i] == 0) {
            if (arg_type_list[i]->getName() != m_arg_list[i]) {
                is_valid = false;
                break;
            }
        } else {
            string par = m_arg_list[i];
            m_arg_list[i] = arg_type_list[i]->getName();
            if (act_map.find(par) == act_map.end()) {
                act_map[par] = arg_type_list[i]->getName();
            } else {
                if (act_map[par] != arg_type_list[i]->getName()) {
                    is_valid = false;
                }
            }
        }
    }
    if (!is_valid) {
        throw SemanticException("the argument of " + fname + " is not matched!");
    }
    if (m_par_arg_mark_list.back()) {
        m_arg_list[m_arg_list.size()-1] = act_map[m_arg_list.back()];
    }

    string range = m_arg_list.back();
    ArgTypeList arg_list = arg_type_list;

    // string key;
    // genStr(arg_list, key);
    // m_func_decl_bucket[key] = (fname+"_"+key+"_"+range);
    // cout << "fun_decl: " << fname << "_" << key << "_" << range << endl; 
    // mk
    z3::sort_vector svec(z3_ctx);
    for (auto arg : arg_list) {
        svec.push_back(z3_buffer.getSort(arg));
    }
    z3::sort srange = z3_buffer.getSort(range);
    return z3_ctx.function(fname.c_str(), svec, srange);

}

void ParFuncType::show() {
    FuncType::show();
    cout << " parameters <";
    for (auto par : m_par_arg_mark_list) {
        cout << par << " ";
    }
    cout << "> ";
}
