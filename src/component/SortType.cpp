/*******************************************
*  @file  SortType.cpp                     *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "component/SortType.h"

extern z3::context z3_ctx;

SortType::operator z3::sort() {
    if (m_sort_name == "Bool") {
        return z3_ctx.bool_sort();
    } else if (m_sort_name == "Int") {
        return z3_ctx.int_sort();
    } else if (m_sort_name == "Float") {
        return z3_ctx.real_sort();
    } else {
        return z3_ctx.uninterpreted_sort(m_sort_name.c_str());
    }
}