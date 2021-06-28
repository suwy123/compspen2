#include "slah_api.h"



//extern z3::context z3_ctx;



bool slah_api::isFun(z3::expr expr, std::string fname) {
	
		if(fname == "emp" && expr.to_string() == "emp") return true; 

        if (expr.is_app() && expr.decl().name().str() == fname) return true;

        return false;
}





z3::expr slah_api::sep(z3::expr spatialFormula1, z3::expr spatialFormula2){

	if(Z3_ast(spatialFormula1)==nullptr) return spatialFormula2;

	if(Z3_ast(spatialFormula2)==nullptr) return spatialFormula1;
	
	z3::context& z3_ctx = spatialFormula1.ctx();

	z3::expr_vector spatialFormulaSet(z3_ctx);

	spatialFormulaSet.push_back(spatialFormula1);
	spatialFormulaSet.push_back(spatialFormula2);

	return newSep(spatialFormulaSet);

}



z3::expr slah_api::newSep(z3::expr_vector spatialFormulaSet){
	
	z3::context& z3_ctx = spatialFormulaSet.ctx();

	z3::sort boolSort = z3_ctx.bool_sort();
	z3::sort range_sort = boolSort;
    z3::sort_vector args_sort(z3_ctx);
    
    z3::expr_vector spatialAtomSet(z3_ctx);
    for(int i=0;i<spatialFormulaSet.size();i++){
    	z3::expr spatialFormula = spatialFormulaSet[i];
    	if(isFun(spatialFormula, "sep")){
    		for (int j=0; j<spatialFormula.num_args(); j++) {
	    		spatialAtomSet.push_back(spatialFormula.arg(j));
	    	}
		}else{
			spatialAtomSet.push_back(spatialFormula);
		}
	}
    
    for (int i=0; i<spatialAtomSet.size(); i++) {
    	args_sort.push_back(boolSort);
    }
    z3::func_decl sep_f = z3_ctx.function("sep", args_sort, range_sort);

    return sep_f(spatialAtomSet);

}



z3::expr slah_api::newBlk(z3::expr x, z3::expr y){
	z3::context& z3_ctx = x.ctx();

	z3::sort boolSort = z3_ctx.bool_sort();

	z3::sort intSort = z3_ctx.int_sort();

	z3::sort range_sort = boolSort;

	z3::sort arg1_sort = intSort;

	z3::sort arg2_sort = intSort;

	z3::func_decl blk_f = z3_ctx.function("blk", arg1_sort, arg2_sort, range_sort);

	return blk_f(x,y);

}



z3::expr slah_api::newPto(z3::expr x, z3::expr y){
	
	z3::context& z3_ctx = x.ctx();

	z3::sort boolSort = z3_ctx.bool_sort();

	z3::sort intSort = z3_ctx.int_sort();

	z3::sort range_sort = boolSort;

	z3::sort arg1_sort = intSort;

	z3::sort arg2_sort = boolSort;

	z3::func_decl pto_f = z3_ctx.function("pto", arg1_sort, arg2_sort, range_sort);

	z3::func_decl data_t = z3_ctx.function("data_t", z3_ctx.int_sort(), boolSort);

	return pto_f(x,data_t(y));

}



z3::expr slah_api::newEmp(z3::context& z3_ctx){

	z3::expr emp = z3_ctx.constant("emp", z3_ctx.bool_sort());

	return emp;

}







z3::check_result slah_api::checkSat(z3::expr phi){
	
	if(Z3_ast(phi) == nullptr) return z3::sat;
	
	z3::context& z3_ctx = phi.ctx();

	if(phi.decl().name().str() != "and"){

    	expr_vector items(z3_ctx);

    	if(Z3_ast(phi) != nullptr) items.push_back(phi);

    	phi = z3::mk_and(items);

	}

	Problem m_problem(z3_ctx);

	m_problem.setLogic("QF_SLAH");

	

	HeapChunk* hck = setHck(z3_ctx);

	m_problem.setHeapChunk(hck);

	

	Predicate* hls = setHls(z3_ctx);

	m_problem.setPredicate(hls);

	

	m_problem.setPhi(phi);


	PASolver sol(z3_ctx);

    sol.setProblem(&m_problem);

    return sol.check_sat();

}



z3::check_result slah_api::checkEnt(z3::expr phi, z3::expr psi){
	
	z3::context& z3_ctx = phi.ctx();

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



	Problem m_problem(z3_ctx);

	m_problem.setLogic("QF_SLAH");

	

	HeapChunk* hck = setHck(z3_ctx);

	m_problem.setHeapChunk(hck);

	

	Predicate* hls = setHls(z3_ctx);

	m_problem.setPredicate(hls);

	

	m_problem.setPhi(phi);

	m_problem.setPsi(psi);

	

	PASolver sol(z3_ctx);

    sol.setProblem(&m_problem);

    return sol.check_entl();

}



z3::expr slah_api::newHck(z3::expr x, z3::expr y, z3::expr v){
	
	z3::context& z3_ctx = x.ctx();

	z3::sort boolSort = z3_ctx.bool_sort();

	z3::sort intSort = z3_ctx.int_sort();

	z3::sort range_sort = boolSort;

	z3::sort arg1_sort = intSort;

	z3::sort arg2_sort = intSort;

	z3::sort arg3_sort = intSort;

	z3::func_decl hck_f = z3_ctx.function("hck", arg1_sort, arg2_sort, arg3_sort, range_sort);

	return hck_f(x,y,v);

}



HeapChunk* slah_api::setHck(z3::context& z3_ctx){

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

	HeapChunk* hck = new HeapChunk(z3_ctx, hckfun, pars, exp, ex_size);

	return hck;

}



z3::expr slah_api::newHls(z3::expr x, z3::expr y, z3::expr v){
	
	z3::context& z3_ctx = x.ctx();

	z3::sort boolSort = z3_ctx.bool_sort();

	z3::sort intSort = z3_ctx.int_sort();

	z3::sort range_sort = boolSort;

	z3::sort arg1_sort = intSort;

	z3::sort arg2_sort = intSort;

	z3::sort arg3_sort = intSort;

	z3::func_decl hls_f = z3_ctx.function("hls", arg1_sort, arg2_sort, arg3_sort, range_sort);

	return hls_f(x,y,v);

}



Predicate* slah_api::setHls(z3::context& z3_ctx){

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

	Predicate* hls_p = new Predicate_SLAH(z3_ctx, hlsfun, pars, base, rec);

	return hls_p;

}




