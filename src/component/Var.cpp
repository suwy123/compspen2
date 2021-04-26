/*******************************************
*  @file  Var.cpp                          *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "component/Var.h"
#include "component/Z3Buffer.h"

extern z3::context z3_ctx;
extern Z3Buffer z3_buffer;

Var::operator z3::expr() {
    sort& s = z3_buffer.getSort(m_sort_type);
    return z3_ctx.constant(m_var_name.c_str(), s); 
}
