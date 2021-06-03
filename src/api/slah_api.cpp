#include "slah_api.h"



extern z3::context z3_ctx;



bool slah_api::is_fun(z3::expr expr, std::string fname) {

        if (expr.is_app() && expr.decl().name().str() == fname) return true;

        return false;

}





z3::expr slah_api::sep(z3::expr spatialFormula1, z3::expr spatialFormula2){

	if(Z3_ast(spatialFormula1)==nullptr) return spatialFormula2;

	if(Z3_ast(spatialFormula2)==nullptr) return spatialFormula1;

	z3::expr_vector spatialAtomSet(z3_ctx);

	if(is_fun(spatialFormula1, "sep") && is_fun(spatialFormula2, "sep")){

	    for (int i=0; i<spatialFormula1.num_args(); i++) {

	    	spatialAtomSet.push_back(spatialFormula1.arg(i));

	    }

	    for (int i=0; i<spatialFormula2.num_args(); i++) {

	    	spatialAtomSet.push_back(spatialFormula2.arg(i));

	    }

	}else if(is_fun(spatialFormula1, "sep")){

	    for (int i=0; i<spatialFormula1.num_args(); i++) {

	    	spatialAtomSet.push_back(spatialFormula1.arg(i));

	    }

	    spatialAtomSet.push_back(spatialFormula2);

	}else if(is_fun(spatialFormula2, "sep")){

	    spatialAtomSet.push_back(spatialFormula1);

	    for (int i=0; i<spatialFormula2.num_args(); i++) {

	    	spatialAtomSet.push_back(spatialFormula2.arg(i));

	    }	

	}else{

	    spatialAtomSet.push_back(spatialFormula1);

	    spatialAtomSet.push_back(spatialFormula2);

	}

	return newSep(spatialAtomSet);

}



z3::expr slah_api::newSep(z3::expr_vector spatialAtomSet){

	z3::sort boolSort = z3_ctx.bool_sort();

	z3::sort range_sort = boolSort;

    z3::sort_vector args_sort(z3_ctx);

    for (unsigned i=0; i<spatialAtomSet.size(); i++) {

    	args_sort.push_back(boolSort);

    }

    z3::func_decl sep_f = z3_ctx.function("sep", args_sort, range_sort);

    return sep_f(spatialAtomSet);

}



z3::expr slah_api::newBlk(z3::expr x, z3::expr y){

	z3::sort boolSort = z3_ctx.bool_sort();

	z3::sort intSort = z3_ctx.int_sort();

	z3::sort range_sort = boolSort;

	z3::sort arg1_sort = intSort;

	z3::sort arg2_sort = intSort;

	z3::func_decl blk_f = z3_ctx.function("blk", arg1_sort, arg2_sort, range_sort);

	return blk_f(x,y);

}



z3::expr slah_api::newPto(z3::expr x, z3::expr y){

	z3::sort boolSort = z3_ctx.bool_sort();

	z3::sort intSort = z3_ctx.int_sort();

	z3::sort range_sort = boolSort;

	z3::sort arg1_sort = intSort;

	z3::sort arg2_sort = boolSort;

	z3::func_decl pto_f = z3_ctx.function("pto", arg1_sort, arg2_sort, range_sort);

	z3::func_decl data_t = z3_ctx.function("data_t", z3_ctx.int_sort(), boolSort);

	return pto_f(x,data_t(y));

}



z3::expr slah_api::newEmp(){

	z3::expr emp = z3_ctx.constant("emp", z3_ctx.bool_sort());

	return emp;

}







z3::check_result slah_api::checkSat(z3::expr phi){

	if(Z3_ast(phi) == nullptr||phi.decl().name().str() != "and"){

	    	expr_vector items(z3_ctx);

	    	if(Z3_ast(phi) != nullptr) items.push_back(phi);

	    	phi = z3::mk_and(items);

	}

	Problem m_problem;

	

	m_problem.setLogic("QF_SLAH");

	

	HeapChunk* hck = setHck();

	m_problem.setHeapChunk(hck);

	

	Predicate* hls = setHls();

	m_problem.setPredicate(hls);

	

	m_problem.setPhi(phi);

	

	PASolver sol;

    sol.setProblem(&m_problem);

    return sol.check_sat();

}



z3::check_result slah_api::checkEnt(z3::expr phi, z3::expr psi){

	if(Z3_ast(phi) == nullptr||phi.decl().name().str() != "and"){

	    	expr_vector items(z3_ctx);

	    	if(Z3_ast(phi) != nullptr) items.push_back(phi);

	    	phi = z3::mk_and(items);

	}

	if(Z3_ast(psi) == nullptr||psi.decl().name().str() != "and"){

	    	expr_vector items(z3_ctx);

	    	if(Z3_ast(psi) != nullptr) items.push_back(psi);

	    	psi = z3::mk_and(items);

	}



	Problem m_problem;

	

	m_problem.setLogic("QF_SLAH");

	

	HeapChunk* hck = setHck();

	m_problem.setHeapChunk(hck);

	

	Predicate* hls = setHls();

	m_problem.setPredicate(hls);

	

	m_problem.setPhi(phi);

	m_problem.setPsi(psi);

	

	PASolver sol;

    sol.setProblem(&m_problem);

    return sol.check_entl();

}



z3::expr slah_api::newHck(z3::expr x, z3::expr y, z3::expr v){

	z3::sort boolSort = z3_ctx.bool_sort();

	z3::sort intSort = z3_ctx.int_sort();

	z3::sort range_sort = boolSort;

	z3::sort arg1_sort = intSort;

	z3::sort arg2_sort = intSort;

	z3::sort arg3_sort = intSort;

	z3::func_decl hck_f = z3_ctx.function("hck", arg1_sort, arg2_sort, arg3_sort, range_sort);

	return hck_f(x,y,v);

}



HeapChunk* slah_api::setHck(){

	string fname = "hck";

	z3::symbol fname_sym = z3_ctx.str_symbol(fname.c_str());//

	z3::sort rangesort = z3_ctx.bool_sort();

	z3::sort_vector args_sort(z3_ctx);

	args_sort.push_back(z3_ctx.int_sort());

	args_sort.push_back(z3_ctx.int_sort());

	args_sort.push_back(z3_ctx.int_sort());

	z3::func_decl hckfun = z3_ctx.function(fname_sym, args_sort, rangesort);

	

	z3::expr_vector pars(z3_ctx);

	z3::expr x = z3_ctx.constant("?x", z3_ctx.int_sort());

	z3::expr y = z3_ctx.constant("?y", z3_ctx.int_sort());

	z3::expr v = z3_ctx.constant("?v", z3_ctx.int_sort());

	pars.push_back(x);

	pars.push_back(y);

	pars.push_back(v);

	

	z3::expr sz = y - x;

	z3::expr pto = newPto(x, sz);

	z3::expr blk = newBlk(x+1, y);

	z3::expr spatialFormula = sep(pto, blk);

	z3::expr exp = sz >= 2 && sz <= v && spatialFormula;

	

	int ex_size=0;

	if (exp.is_quantifier()) {

		ex_size=Z3_get_quantifier_num_bound(z3_ctx,Z3_ast(exp));

        	exp = exp.body();

	}

	HeapChunk* hck = new HeapChunk(hckfun, pars, exp, ex_size);

	return hck;

}



z3::expr slah_api::newHls(z3::expr x, z3::expr y, z3::expr v){

	z3::sort boolSort = z3_ctx.bool_sort();

	z3::sort intSort = z3_ctx.int_sort();

	z3::sort range_sort = boolSort;

	z3::sort arg1_sort = intSort;

	z3::sort arg2_sort = intSort;

	z3::sort arg3_sort = intSort;

	z3::func_decl hls_f = z3_ctx.function("hls", arg1_sort, arg2_sort, arg3_sort, range_sort);

	return hls_f(x,y,v);

}



Predicate* slah_api::setHls(){

	string fname = "hls";

	z3::symbol fname_sym = z3_ctx.str_symbol(fname.c_str());//

    z3::sort rangesort = z3_ctx.bool_sort();

    z3::sort_vector args_sort(z3_ctx);

    args_sort.push_back(z3_ctx.int_sort());

    args_sort.push_back(z3_ctx.int_sort());

	args_sort.push_back(z3_ctx.int_sort());

	z3::func_decl hlsfun = z3_ctx.function(fname_sym, args_sort, rangesort);

	

	z3::expr_vector pars(z3_ctx);

	z3::expr x = z3_ctx.constant("?x", z3_ctx.int_sort());

	z3::expr y = z3_ctx.constant("?y", z3_ctx.int_sort());

	z3::expr v = z3_ctx.constant("?v", z3_ctx.int_sort());

	pars.push_back(x);

	pars.push_back(y);

	pars.push_back(v);

	

	z3::expr base = x == y;

	

	z3::expr w = z3_ctx.constant("?w", z3_ctx.int_sort());

	z3::expr hck = newHck(x, w, v);

	z3::expr hls = newHls(w, y, v);

	z3::expr body = sep(hck, hls);

	z3::expr rec = z3::exists(w, body);



	Predicate* hls_p = new Predicate_SLAH(hlsfun, pars, base, rec);

	return hls_p;

}




