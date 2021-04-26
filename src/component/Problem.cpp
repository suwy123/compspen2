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
#include "component/Z3Buffer.h"
#include "Types.h"

extern z3::context z3_ctx;
extern Z3Buffer z3_buffer;

Problem::Problem():
    m_phi(z3_ctx), m_psi(z3_ctx), m_pred(nullptr), m_hck(nullptr) {}

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


