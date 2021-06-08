/*******************************************
*  @file  CheckSatParser.cpp               *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2019-2-18                     *
*                                          *
*******************************************/

#include "parser/CheckSatParser.h"
#include "solver_slid_set/SepSolver.h"
#include "solver_slid_int/listsolver.h"
#include "solver_slid_int/alistsolver.h"
#include "solver_slah/PASolver.h"
//#include "component/Z3Buffer.h"

extern SyntaxErrorTable SYNTAX_ERROR_INFO;
//extern Z3Buffer z3_buffer;

void CheckSatParser::parse(Parser& parser) {
    parser.checkNext(RIGHT_PAREN, SYNTAX_ERROR_INFO[RIGHT_PAREN]);

    z3_buffer.setVarEnv(parser);

    Problem* problem = parser.getProblem();
    problem->show();
    if(problem->getLogic()=="QF_SLID_SET"){
    	SepSolver ss(z3_ctx,z3_buffer);
    	ss.setProblem(problem);
    	ss.solve();
	}else if(problem->getLogic()=="QF_SLID_INT"){
		Predicate_SLID_INT *pred = dynamic_cast<Predicate_SLID_INT *>(problem->getPredicate()); 
		if (pred->get_pars().size() == 2) {
            alistsolver ss(z3_ctx, problem);
            ss.solve();
        } else {
            listsolver ss(z3_ctx, problem);
			ss.solve();
        }
	}else if(problem->getLogic()=="QF_SLAH"){
    	PASolver ss(z3_ctx);
    	ss.setProblem(problem);
    	ss.solve();
	}else{
		std::cout<<"No solver for the logic"<<std::endl;
	}
}
