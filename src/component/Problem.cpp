/*******************************************
*  @file  Problem.cpp                      *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2019-2-18                     *
*                                          *
*******************************************/

#include "component/Problem.h"
#include "Types.h"

//z3::context z3_ctx;

Problem::Problem(z3::context& ctx):
    z3_ctx(ctx), m_phi(ctx), m_psi(ctx), m_pred(nullptr), m_hck(nullptr) {}

Problem::~Problem() {
    if (m_pred != nullptr) {
        delete m_pred;
    }
    if (m_hck != nullptr) {
        delete m_hck;
    }
}

//void Problem::setSolver(SepSolver* ss) {
//    m_ss = ss;
//}

void Problem::show() {
	cout << "logic: " << m_logic << endl;
    cout << "Heap: \n";
    cout << m_heap.first->getName() << " -> " << m_heap.second->getName() << endl;
    if(m_logic == "QF_SLAH"){
    	cout << "hck: \n";
    	m_hck->show();
	}
    cout << "predicate: \n";
    m_pred->show();

    cout << "phi: " << m_phi << endl;
    // getAbsPhi();

    cout << "psi: " << m_psi << endl;
}


bool Problem::isSat() {
    return Z3_ast(m_psi) == nullptr;
}


