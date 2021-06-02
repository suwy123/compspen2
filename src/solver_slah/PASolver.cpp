#include "solver_slah/PASolver.h"
#include "component/Predicate_SLAH.h"
#include "exception/SemanticException.h"
#include "solver_slid_int/expr_tool.h"
//#include "time_tool.h"

extern z3::context z3_ctx;

PASolver::PASolver() 
: m_problem(nullptr), z3_sol(z3_ctx), k_index(0), z_newvar_index(0), w_newvar_index(0), ex_newvar_index(0){}

void PASolver::solve() {
	check_preds();
    z3::check_result result;
    if (m_problem->isSat()) {
        result = check_sat();
    } else {
        result = check_entl();
    }
    if (result == z3::unsat) {
        std::cout << "The result: unsat" << std::endl;
    } else {
        std::cout << "The result: sat" << std::endl;
    }
}


/**
 * check whether all predicate definitions are well-formed
 */
void PASolver::check_preds() {
//std::cout<<"check predicates:----------------------------"<<std::endl;
	//1.check hlsdef
    Predicate_SLAH *pdef = dynamic_cast<Predicate_SLAH *>(m_problem->getPredicate()); 
	z3::expr hck_in_pdef=pdef->get_hck();
	z3::expr rec_app=pdef->get_rec_app();
	int exsize=pdef->get_ex_size();
	int zsize=pdef->get_z_size();
	int vsize=pdef->get_v_size();
	if(zsize<0||vsize<0){
		throw SemanticException("invalid number of parameters in recursive rules of predicate. \n");
	}
	
	//hck后面的参数要等于后面归纳谓词的前面的参数 
	for(int j=0;j<exsize;j++){
		if(hck_in_pdef.arg(exsize+j).to_string() != rec_app.arg(j).to_string()){
			throw SemanticException("tail of hck in rec predicate def must equal to head of rec_app. \n");
		}else if(hck_in_pdef.arg(exsize+j).to_string().find("(:var")!=0){
			throw SemanticException("tail of hck must be exist parameter. \n");
		}
	}
	
	
	//2.check hckdef
	HeapChunk* hckdef=m_problem->getHeapChunk();
	hckdef->set_z_size(zsize);
	hckdef->set_v_size(vsize);
	
//hckdef->show();
	
	//the first atom is x \mapsto y-x
	if(hckdef->get_space()[0].decl().name().str() == "pto"){
		z3::expr x = hckdef->get_space()[0].arg(0);
		z3::expr ref = hckdef->get_space()[0].arg(1);
		z3::expr u = ref.arg(0);
		z3::expr sz = hckdef->get_pars()[exsize] - hckdef->get_pars()[0];
		if(x.to_string() != hckdef->get_pars()[0].to_string() || sz.to_string() != u.to_string()){
			std::string info = "'x |-> y-x' is expected in the first spatial atom of "+hckdef->get_name()+". \n";
        	throw SemanticException(info);
		}
	}else{
		std::string info = "'x |-> y-x' is expected in the first spatial atom of "+hckdef->get_name()+". \n";
        throw SemanticException(info);
	}
	
	//consecutive
	for(int j=0;j<hckdef->get_space().size()-1;j++){
		z3::expr atom1=hckdef->get_space()[j];
		z3::expr atom2=hckdef->get_space()[j+1];
		z3::expr con = tail(atom1) == head(atom2);
		z3_sol.reset();
		z3_sol.add(!con);
		if(z3_sol.check() == z3::sat){
			std::string info = "The spatial atoms of "+hckdef->get_name()+" is expected to be consecutive. \n";
        	throw SemanticException(info);
		}
	}
   
    //the last address of spatial part should be y
    z3::expr last_atom = hckdef->get_space()[hckdef->get_space().size()-1];
    if(tail(last_atom).to_string() != hckdef->get_pars()[exsize].to_string()){
    	std::string info = "invalid last spatial atom of "+hckdef->get_name()+". \n";
        throw SemanticException(info);
	}

	//data should be true or n1<y-x<n2 or n<y-x<v or \bigvee(z=n \wedge z'=n')
	hckdef->make_fts();
	hckdef->set_v();
	//m_ctx.set_hck_w(hckdef_index);
	//std::cout<<Z3_get_index_value(z3_ctx(),Z3_ast(hck.arg(1)))<<std::endl;
	//Z3_symbol ex_sym = Z3_get_quantifier_bound_name(z3_ctx(),Z3_ast(rec_rule_exp),0);
	//std::string ex_name=Z3_get_symbol_string(z3_ctx(), ex_sym);
	//int ex_size=Z3_get_quantifier_num_bound(z3_ctx(),Z3_ast(rec_rule_exp));
    
}

z3::expr_vector PASolver::get_disjunctive_normal_form(z3::expr_vector formula_set){
//std::cout<<formula_set<<std::endl;
	z3::expr_vector disjunct_set(z3_ctx);
	for(int j=0;j<formula_set.size();j++){
		z3::expr_vector item_set(z3_ctx);
		z3::expr formula = formula_set[j];
		z3::expr_vector sub_disjunct_set(z3_ctx);
		if(is_fun(formula, "and")){
			bool pi_and_porp = false;
			for(int i=0;i<formula.num_args();i++){
				if(is_fun(formula.arg(i), "or")){
					pi_and_porp = true;
					z3::expr_vector sub_formula_set(z3_ctx);
					for(int k=0;k<formula.arg(i).num_args();k++){
						sub_formula_set.push_back(formula.arg(i).arg(k));
					}
					sub_disjunct_set = get_disjunctive_normal_form(sub_formula_set);
				}else{
					item_set.push_back(formula.arg(i));
				}
			}
			if(pi_and_porp){
				for(int k=0;k<sub_disjunct_set.size();k++){
					item_set.push_back(sub_disjunct_set[k]);
					disjunct_set.push_back(mk_and(item_set));
					item_set.pop_back();
				}
			}else{
				disjunct_set.push_back(formula);
			}
		}else if(is_fun(formula, "or")){
			z3::expr_vector sub_formula_set(z3_ctx);
			for(int i=0;i<formula.num_args();i++){
				sub_formula_set.push_back(formula.arg(i));
			}
			sub_disjunct_set = get_disjunctive_normal_form(sub_formula_set);
			for(int i=0;i<sub_disjunct_set.size();i++){
				disjunct_set.push_back(sub_disjunct_set[i]);
			}
		}else{
			disjunct_set.push_back(formula);
		}
	}
//std::cout<<"result-------------"<<std::endl;
//std::cout<<disjunct_set<<std::endl;
	return disjunct_set;
}

/**
 * check sat, negf in m_ctx
 * or check entl, negf |= posf
 * @return z3::check_result
 */
z3::check_result PASolver::check_sat() {
    z3::expr formula = m_problem->getPhi();
    
	z3::expr_vector formula_set(z3_ctx);
	formula_set.push_back(formula);
	z3::expr_vector disjunct_set = get_disjunctive_normal_form(formula_set);
    
    for(int i = 0;i<disjunct_set.size();i++){
    	z3::expr disjunct = disjunct_set[i];
    	z3::expr space(z3_ctx);
	    z3::expr data(z3_ctx); 
	    get_data_space(disjunct, data, space);//add nil = 0 in data
	    remove_emp(space);
	   
//std::cout<<"data "<<i<<": "<<data<<std::endl;
//std::cout<<"space "<<i<<": "<<space<<std::endl;

	    z3::expr f_abs = get_abstraction(data, space);
	    
	    z3_sol.reset();
	    z3_sol.add(f_abs);
	    z3::check_result result = z3_sol.check();
	    if(result == z3::sat) return result;
	}
    return z3::unsat;
}

z3::model PASolver::get_model(){
	z3::model m(z3_ctx);
	return m;
}

z3::model PASolver::get_model(){
	z3::model m(z3_ctx);
	return m;
}

/**
 * check entl, negf |= posf
 * @return z3::check_result
 */
z3::check_result PASolver::check_entl() {//suppose no emp
	z3::expr phi = m_problem->getPhi();
    z3::expr_vector formula_setA(z3_ctx);
	formula_setA.push_back(phi);
	z3::expr_vector disjunct_setA = get_disjunctive_normal_form(formula_setA);
	
	z3::expr psi = m_problem->getPsi();
    z3::expr_vector formula_setB(z3_ctx);
	formula_setB.push_back(psi);
	z3::expr_vector disjunct_setB = get_disjunctive_normal_form(formula_setB);

//std::cout<<disjunct_setB<<std::endl;
	
	if(disjunct_setB.size()>1){
		std::cout<<"psi has disjunction, so if the output is sat, it is not accurate!!!"<<std::endl;
	}
	
	for(int j=0;j<disjunct_setA.size();j++){
		z3::check_result sub_result = z3::sat;
		z3::expr formulaA = disjunct_setA[j];
		z3::expr spaceA(z3_ctx);
	    z3::expr dataA(z3_ctx);
	    get_data_space(formulaA, dataA, spaceA);
	    remove_emp(spaceA);
		for(int k=0;k<disjunct_setB.size();k++){
			z3::expr formulaB = disjunct_setB[k];
			z3::expr_vector ex_var_set(z3_ctx);
		    if(formulaB.is_quantifier()){
		    	//std::cout<<Z3_get_index_value(z3_ctx(),Z3_ast(hck.arg(1)))<<std::endl;
				//Z3_symbol ex_sym = Z3_get_quantifier_bound_name(z3_ctx(),Z3_ast(rec_rule_exp),0);
				//std::string ex_name=Z3_get_symbol_string(z3_ctx(), ex_sym);
				int ex_size=Z3_get_quantifier_num_bound(z3_ctx,Z3_ast(formulaB));
				for(int i=0;i<ex_size;i++){
					Z3_symbol ex_sym = Z3_get_quantifier_bound_name(z3_ctx,Z3_ast(formulaB),i);
					std::string ex_name=Z3_get_symbol_string(z3_ctx, ex_sym);
		    		z3::expr ex_var = z3_ctx.constant(z3_ctx.str_symbol(ex_name.c_str()),z3_ctx.int_sort());
		    		ex_var_set.push_back(ex_var);
				}
				formulaB=formulaB.body();
				z3::expr_vector emp(z3_ctx);
				par2arg(formulaB,emp,emp,ex_var_set);
			}
		    
		    z3::expr spaceB(z3_ctx);
		    z3::expr dataB(z3_ctx);
		    get_data_space(formulaB, dataB, spaceB);
		    remove_emp(spaceB);
std::cout<<"-----------phi_"<<j<<" |= psi_"<<k<<"------------"<<std::endl;
std::cout<<dataA<<" : "<<spaceA<<std::endl;
std::cout<<dataB<<" : "<<spaceB<<std::endl;
		    
		    z3::expr absA = get_abstraction(dataA, spaceA);
		    z3_sol.reset();
		    z3_sol.add(absA);
		    if(z3_sol.check() == z3::unsat) {
		    	std::cout<<"abs(A) is unsat"<<std::endl;
		    	sub_result = z3::unsat;//entl is true when absA is unsat
		    	break;
		    }
		    
		    z3::expr absB = get_abstraction(dataB, spaceB);
		    z3_sol.reset();
		    if(ex_var_set.size()!=0){
		    	z3_sol.add(absA && (!exists(ex_var_set, absB)));
			}else{
				z3_sol.add(absA && (!absB));
			}
			if(z3_sol.check() == z3::sat){
		    	std::cout<<"Abs(A) entl Abs(B) is false"<<std::endl;
				continue;//return z3::sat;//entl is false when Abs(A) \vDash Abs(B) is false
			}
			
			if(Z3_ast(spaceA) == 0){//if spaceA is null,add ssep to it for consistency
				z3::expr_vector args0(z3_ctx);
				spaceA = new_sep(args0);
			}
		//	if (spaceA.decl().name().str() != "sep"){//(tobool (blk x1 x2)) if spaceA has only one atom,add ssep to it for consistency
		//		z3::expr_vector args1(z3_ctx);
		//		args1.push_back(spaceA.arg(0));
		//		spaceA = new_sep(args1);
		//	}
			if(Z3_ast(spaceB) == 0){//if spaceB is null,add ssep to it for consistency
				z3::expr_vector args0(z3_ctx);
				spaceB = new_sep(args0);
			}
		//	if (spaceB.decl().name().str() != "sep"){//if spaceB has only one atom,add ssep to it for consistency
		//		z3::expr_vector args1(z3_ctx);
		//		args1.push_back(spaceB.arg(0));
		//		spaceB = new_sep(args1);
		//	}
			
			if(m_problem->getHeapChunk()!=nullptr){
//std::cout<<"------changehck--------------"<<std::endl;
				change_hck(dataA,spaceA);
//std::cout<<"dataA: "<<dataA<<std::endl;
//std::cout<<"spaceA: "<<spaceA<<std::endl; 
				change_hck(dataB,spaceB,ex_var_set);
//std::cout<<"dataB: "<<dataB<<std::endl;
//std::cout<<"spaceB: "<<spaceB<<std::endl;
//std::cout<<"ex_var_set: " <<ex_var_set<<std::endl;
			}
		
			z3::expr_vector headsA = get_space_heads(spaceA);
		    z3::expr_vector tailsA = get_space_tails(spaceA);
		    z3::expr_vector tailsB = get_space_tails(spaceB);
		    z3::expr_vector headsB = get_space_heads(spaceB);
			//adds:headsA,tailsA,tailsB without repitition
			z3::expr_vector adds(z3_ctx);
			for(int i = 0; i < headsA.size(); i++){
				adds.push_back(headsA[i]);
			}
			for(int i = 0; i < tailsA.size(); i++){
				if(find_index(adds,tailsA[i]) == -1) adds.push_back(tailsA[i]);
			}
			for(int i = 0; i < tailsB.size(); i++){
				if(find_index(adds,tailsB[i]) == -1) adds.push_back(tailsB[i]);
			}
			for(int i = 0; i < headsB.size(); i++){
				if(find_index(adds,headsB[i]) == -1) adds.push_back(headsB[i]);
			}
		//clock_t start, end;
		//start = clock();
			rel.init(adds,absA);
		//rel.print();
		//end = clock();
		//std::cout <<"get_init_relations:"<< (double)(end - start)/(double)CLOCKS_PER_SEC <<std::endl;
		//system("read REPLY");
		
			z3::expr_vector args_emp(z3_ctx);
		    z3::expr spaceAhls(z3_ctx);
			spaceAhls = new_sep(args_emp);
		    z3::expr spaceBhls(z3_ctx);
			spaceBhls = new_sep(args_emp);
			try{
				z3::expr_vector pi_ex_set(z3_ctx);
				z3::expr pi_ex=z3_ctx.bool_val(true);
				pi_ex_set.push_back(pi_ex);
		    	pre_match(absA, spaceA, absB, spaceB, ex_var_set, pi_ex_set);//因为里面match_atom时需要全部的absA，如果只有dataA就会丢失一些约束。传absB是因为后面加入所有存在变量取值的约束后还需要看整个蕴涵是否成立 
		   	    pre_P(spaceA, dataA, spaceB, absB, absA, spaceAhls, spaceBhls, ex_var_set, pi_ex_set);
			}catch(int e){
				continue;//return z3::sat;
			}
			
			sub_result = z3::unsat;//return z3::unsat;
			break;
		}
		if(sub_result == z3::sat){
			return z3::sat;
		}
	}
	return z3::unsat;
}

z3::expr_vector PASolver::get_conjunct(z3::expr formula){
	z3::expr_vector conjunct_set(z3_ctx);
	if(!is_fun(formula, "and")){//disjunct中可能是原子或空间公式 
		conjunct_set.push_back(formula);
		return conjunct_set;
	}
	Predicate_SLAH *pdef = dynamic_cast<Predicate_SLAH *>(m_problem->getPredicate());
	for(int i=0;i<formula.num_args();i++){
    	if(is_fun(formula.arg(i),"and")){
    		z3::expr_vector sub_conjunct_set = get_conjunct(formula.arg(i));
    		for(int j=0;j<sub_conjunct_set.size();j++){
    			conjunct_set.push_back(sub_conjunct_set[j]);
			}
		}else if(is_fun(formula.arg(i),"<")||is_fun(formula.arg(i),"<=")||is_fun(formula.arg(i),">")||is_fun(formula.arg(i),">=")
	       ||is_fun(formula.arg(i),"=")||is_fun(formula.arg(i),"distinct")
		   ||formula.arg(i).to_string()=="emp"||is_fun(formula.arg(i),"pto")||is_fun(formula.arg(i),"blk")
		   ||is_fun(formula.arg(i),m_problem->getHeapChunk()->get_name())||is_fun(formula.arg(i),pdef->get_name())
		   ||is_fun(formula.arg(i),"sep")){
		   		conjunct_set.push_back(formula.arg(i));
		}else{
			std::string info = "invalid conjunct of assert";
        	throw SemanticException(info);
		}
	}
	return conjunct_set;
}

void PASolver::get_data_space(z3::expr &formula, z3::expr &data, z3::expr &space) {
	//formula在assertparser时处理过，若只有一个原子atom会改写成(and atom)  ，//disjunct中可能是原子或空间公式 
	if(Z3_ast(formula)==nullptr) return;
	expr_vector data_items(z3_ctx);
    data_items.push_back(z3_ctx.int_const("nil") == 0); // nil == 0
    
    Predicate_SLAH *pdef = dynamic_cast<Predicate_SLAH *>(m_problem->getPredicate()); 
    
    z3::expr_vector conjunct_set = get_conjunct(formula);
    for(int i=0;i<conjunct_set.size();i++){
    	if(is_fun(conjunct_set[i],"pto")||is_fun(conjunct_set[i],"blk")||is_fun(conjunct_set[i],m_problem->getHeapChunk()->get_name())
		||is_fun(conjunct_set[i],pdef->get_name())||conjunct_set[i].to_string()=="emp"||is_fun(conjunct_set[i],"sep")){
			if(Z3_ast(space)==nullptr){
				if(is_fun(conjunct_set[i],"sep")){
					space = conjunct_set[i];
				}else{
					expr_vector space_items(z3_ctx);
					space_items.push_back(conjunct_set[i]);
					space = new_sep(space_items);
				}
			}else{
				std::string info = "invalid conjunct of spatial formula";
        		throw SemanticException(info);
			}
		}else{
			data_items.push_back(conjunct_set[i]);
		}
	}
    
    data = mk_and(data_items);
}

void PASolver::remove_emp(z3::expr& space){
	if(Z3_ast(space)==nullptr) return;
	z3::expr newSpace(z3_ctx);
	if(space.to_string() == "emp") space = newSpace;
	if(space.is_app()&&is_fun(space,"sep")){
		z3::expr_vector newSpace_args(z3_ctx);
		for(int i=0;i<space.num_args();i++){
			if(space.arg(i).to_string()!="emp"){
				newSpace_args.push_back(space.arg(i));
			}
		}
		if(newSpace_args.size()==0) space=newSpace;
		else space = new_sep(newSpace_args);
	}
}

z3::expr PASolver::head(z3::expr atom){
	z3::expr source(z3_ctx);
//	if(atom.to_string() == "emp"){
//		return source;
//	}
    source = atom.arg(0);
	return source;
}

z3::expr PASolver::tail(z3::expr atom){
	z3::expr dest(z3_ctx);
//	if(atom.to_string() == "emp"){
//		return dest;
//	}else 
	if(expr_tool::is_fun(atom, "pto")){
		dest = atom.arg(0);
		return dest+1;
	}else if(expr_tool::is_fun(atom, "blk")){
		dest = atom.arg(1);
		return dest;
	}else{//hck or P
		if(ishck(atom)){
			HeapChunk* hckdef=m_problem->getHeapChunk();
			int zsize=hckdef->get_z_size();
			dest = atom.arg(zsize+1);
		}else{
			if(isP(atom)){
				Predicate_SLAH *pdef = dynamic_cast<Predicate_SLAH *>(m_problem->getPredicate()); 
				int zsize=pdef->get_z_size();
				dest = atom.arg(zsize+1);
			}else{
				std::string info = "fail to get tail of "+atom.decl().name().str()+". \n";
            	throw SemanticException(info);
			}
		}
		
		return dest;
	}
}

z3::expr_vector PASolver::get_space_heads(z3::expr space){
	z3::expr_vector heads(z3_ctx);
	for (int i=0; i<space.num_args(); i++) {
		z3::expr atom = space.arg(i);
		if(Z3_ast(head(atom))!=0) heads.push_back(head(atom));//Z3_ast(head(atom))==0:atom is emp
	}
	return heads;
}

z3::expr_vector PASolver::get_space_tails(z3::expr space){
	z3::expr_vector tails(z3_ctx);
	for (int i=0; i<space.num_args(); i++) {
		z3::expr atom = space.arg(i);
		if(Z3_ast(tail(atom))!=0) tails.push_back(tail(atom));//Z3_ast(head(atom))==0:atom is emp
	}
	return tails;
}

z3::expr_vector PASolver::get_space_args(z3::expr space){
	z3::expr_vector args(z3_ctx);
	for (int i=0; i<space.num_args(); i++) {
		z3::expr atom = space.arg(i);
		args.push_back(atom);
	}
	return args;
}

int PASolver::find_index(z3::expr_vector &addset, z3::expr add){
	for(int i = 0; i < addset.size(); i++){
		if(add.to_string() == addset[i].to_string()) return i;
	}
	return -1;//not found
}

int PASolver::find_index2(std::vector<z3::expr_vector> &adds, z3::expr_vector add){
	for(int i = 0; i < adds.size(); i++){
		bool is_eq = true;
		for(int j=0;j<add.size();j++){
			bool exist = false;
			for(int k = 0; k < adds[i].size();k++){
				if(adds[i][k].to_string() == add[j].to_string()){
					exist = true;
					break;
				}
			}
			if(!exist){
				is_eq = false;
				break;
			}
		}
		if(is_eq) return i;
	}
	return -1;
}

bool PASolver::is_in_sortset(std::vector<std::vector<z3::expr_vector>> &sortset, std::vector<z3::expr_vector> &sort){
	for(int i = 0; i < sortset.size(); i++){
		bool eq = true;
		for(int j = 0; j < sort.size(); j++){
			if(find_index2(sortset[i],sort[j]) != j){
				eq = false;
				break;
			}
		}
		if(eq) return true;
	}
	return false;
}

z3::expr_vector PASolver::get_eq_class(std::vector<z3::expr_vector> &sort, z3::expr &add){
	z3::expr_vector eq_class(z3_ctx);
	for(int i = 0;i<sort.size();i++){
		if(find_index(sort[i],add)!=-1) return sort[i];
	}
	return eq_class;
}

int PASolver::get_size(std::vector<z3::expr_vector> &adds){
	int size = 0;
	for(int i = 0; i < adds.size(); i++){
		size += adds[i].size();
	}
	return size;
}

z3::expr PASolver::new_sep(z3::expr_vector args){
	z3::sort range = z3_ctx.bool_sort();
    z3::sort_vector domains(z3_ctx);
    for (unsigned i=0; i<args.size(); i++) {
    	domains.push_back(range);
    }
    z3::func_decl sep_f = z3_ctx.function(z3_ctx.str_symbol("sep"), domains, range);
    return sep_f(args);
}

z3::expr PASolver::newblk(z3::expr source, z3::expr dest){
	z3::sort range = z3_ctx.bool_sort();
	z3::func_decl blk_f = z3_ctx.function("blk", z3_ctx.int_sort(), z3_ctx.int_sort(), range);
	return blk_f(source,dest);
}

/**
 * get abstraction of formula
 * @param formula :
 * @param space : the space part of formula
 * @param new_bools : aux output new bool vars
 * @return
 */
z3::expr PASolver::get_abstraction(z3::expr& data, z3::expr& space) {
    z3::expr abs = data;
    
    if(Z3_ast(space) == 0) return abs;

    // 1.3 space part
    abs = abs && abs_space(space);

    // 1.4 sep (\phi_star)
    abs = abs && abs_phi_star(space);

    return abs;
}

z3::expr PASolver::abs_space(z3::expr &space) {
    // 1.space atoms -> abs (\phi_sigma)
    z3::expr f_abs(z3_ctx);
    z3::expr b_false = z3_ctx.bool_val(false);
    for (int i=0; i<space.num_args(); i++) {
        //1.1 fetch atom
        z3::expr atom = space.arg(i);
        z3::expr atom_f = pred2abs(atom);
        if (atom_f.hash() == b_false.hash()) return b_false;

        // 1.5 add atom_f to f_abs
        if (Z3_ast(f_abs) == 0) {
            f_abs = atom_f;
        } else {
            f_abs = f_abs && atom_f;
        }
    }

    return f_abs;
}

bool PASolver::ishck(z3::expr atom){			
	if(atom.decl().name().str() == m_problem->getHeapChunk()->get_name()){
		return true;
	}
	return false;
}

//int PASolver::hck_index(z3::expr atom){
//	for(int j=0;j<m_ctx.pred_hck_size();j++){			
//		if(atom.decl().name().str() == m_ctx.get_pred_hck(j).get_pred_name()){
//			return j;
//		}
//	}
//	return -1;
//}

//bool PASolver::isP(z3::expr atom,Predicate_SLAH *pdef){			
//	if(atom.decl().name().str() == m_ctx.get_pred(j).get_pred_name()){
//		pdef=m_ctx.get_pred(j);
//		return true;
//	}
//	return false;
//}

bool PASolver::isP(z3::expr atom){	
	Predicate_SLAH *pdef = dynamic_cast<Predicate_SLAH*>(m_problem->getPredicate()); 		
	if(atom.decl().name().str() == pdef->get_name()){
		return true;
	}
	return false;
}

/**
 * atom in formula to abstraction
 * @param  atom [the atom in formula, like p(Z1, mu; Z2, nu, chi) or (pto Z (*))]
 * @param  i    [the index in formula]
 * @param new_bools [new bool vars]
 * @return      [the abstraction]
 */
z3::expr PASolver::pred2abs(z3::expr &atom) {//solver中定义的纯虚函数 
	
    //if (atom.to_string() == "emp") return z3_ctx.bool_val(true);//emp在CommandParser::_parseExpr()中被处理了 
    
    z3::expr_vector args(z3_ctx);
	for(int j=0;j<atom.num_args();j++){
		args.push_back(atom.arg(j));	
	}
	
	z3::expr source = head(atom);
	z3::expr dest = tail(atom);
	
    z3::expr abs(z3_ctx);
    if (expr_tool::is_fun(atom, "pto")) {
        abs = source > 0;
    }else if(expr_tool::is_fun(atom, "blk")){
    	abs = (source > 0) && (source < dest);
	}else{
		if(ishck(atom)){
			HeapChunk* hckdef=m_problem->getHeapChunk();
			//形参用实参替换 data部分 
			z3::expr data = hckdef->get_data();
			pi_remove_ex(data);
			par2arg(data,hckdef->get_pars(),args);
			int minsize=hckdef->get_space().size();
			abs = (source > 0) && ((dest - source) >= minsize) && data;
		}else{//P
    		Predicate_SLAH *pdef = dynamic_cast<Predicate_SLAH*>(m_problem->getPredicate()); 
    		//isP(atom,pdef);
    		int zsize=pdef->get_z_size();
    		z3::expr emp_abs=z3_ctx.bool_val(true);
    		for(int j=0;j<zsize+1;j++){
    			emp_abs=(emp_abs && (atom.arg(j) == atom.arg(1+zsize+j)));
			}
			
			z3::expr summ(z3_ctx);
			//int h_i=hck_index(pdef.get_rec_rule_hls(0).get_hck());
			m_problem->getHeapChunk()->get_summ(summ,k_index,args);
    		
            z3::expr or_0 = ((source < dest) && summ && (atom.arg(0) > 0));
            z3::expr or_1 = ((source == dest) && emp_abs && (atom.arg(0) > 0));
            abs = or_0 || or_1;
		}
	}
    return abs;
}


void PASolver::par2arg(z3::expr &exp,z3::expr_vector pars,z3::expr_vector args){
	if(exp.is_const()){
		for(int i=0;i<pars.size();i++){
			if(exp.decl().name().str()==pars[i].decl().name().str()){
				exp = args[i];
				break;
			}
		}
	}else{
		z3::expr_vector tmp_pars(z3_ctx);
		for(int i=0;i<exp.num_args();i++){
			z3::expr tmp=exp.arg(i);
			par2arg(tmp,pars,args);
			tmp_pars.push_back(tmp);
		}
		exp=exp.decl()(tmp_pars);
	}
}

//replace pars with args and replace existential variable with exargs
void PASolver::par2arg(z3::expr &exp,z3::expr_vector pars,z3::expr_vector args,z3::expr_vector exargs){
	if(exp.is_var()){//existential variable
		int index=Z3_get_index_value(z3_ctx,Z3_ast(exp));
		exp=exargs[index];
	}else if(exp.is_const()){
		for(int i=0;i<pars.size();i++){
			if(exp.decl().name().str()==pars[i].decl().name().str()){
				exp = args[i];
				break;
			}
		}
	}else{
		z3::expr_vector tmp_pars(z3_ctx);
		for(int i=0;i<exp.num_args();i++){
			z3::expr tmp=exp.arg(i);
			par2arg(tmp,pars,args,exargs);
			tmp_pars.push_back(tmp);
		}
		exp=exp.decl()(tmp_pars);
	}
}


z3::expr PASolver::abs_phi_star(z3::expr &space) {
	z3::expr phi_star = z3_ctx.bool_val(true);
    z3::expr b_false = z3_ctx.bool_val(false);
    
    for (int i=0; i<space.num_args()-1; i++) {
    	for(int j = i + 1; j<space.num_args(); j++){
    		z3::expr atom1 = space.arg(i);
    		z3::expr atom2 = space.arg(j);
    		
    		
    		z3::expr phi(z3_ctx);
    		
			if (isP(atom1) && isP(atom2)){
				phi = head(atom1)==tail(atom1) || head(atom2)==tail(atom2) || tail(atom2)<=head(atom1) || tail(atom1)<=head(atom2);
			}else if(isP(atom1) && !isP(atom2)){
				phi = head(atom1)==tail(atom1) || tail(atom2)<=head(atom1) || tail(atom1)<=head(atom2);
			}else if(!isP(atom1)&& isP(atom2)){
				phi = head(atom2)==tail(atom2) || tail(atom2)<=head(atom1) || tail(atom1)<=head(atom2);
			}else{
				phi = tail(atom2)<=head(atom1) || tail(atom1)<=head(atom2);
			}
			phi_star = phi_star && phi;
    		
		}
    }
    return phi_star;
}


void PASolver::change_hck(z3::expr &data,z3::expr &space){
	z3::expr_vector args(z3_ctx);
	for(int i=0;i<space.num_args();i++){
		z3::expr atom = space.arg(i);
		if(ishck(atom)){
			HeapChunk* hckdef = m_problem->getHeapChunk();
			z3::expr_vector atom_args(z3_ctx);
			for(int j=0;j<atom.num_args();j++){
				atom_args.push_back(atom.arg(j));
			}
			
			z3::expr_vector new_ex_set(z3_ctx);
			for(int j=0;j<hckdef->get_ex_size();j++){
				std::string ex_newvar_name = "[ex,"+std::to_string(ex_newvar_index)+"]";ex_newvar_index++;
		    	z3::expr ex_newvar = z3_ctx.constant(z3_ctx.str_symbol(ex_newvar_name.c_str()),z3_ctx.int_sort());
		    	new_ex_set.push_back(ex_newvar);
			}
			
			//change data
			z3::expr hck_data = hckdef->get_data();
			par2arg(hck_data,hckdef->get_pars(),atom_args,new_ex_set);
			data = data && hck_data;
			
			//change spatial atoms
			z3::expr_vector hck_space = hckdef->get_space();
			for(int j=0;j<hck_space.size();j++){
				z3::expr hck_space_atom = hck_space[j];
				par2arg(hck_space_atom,hckdef->get_pars(),atom_args,new_ex_set);
				args.push_back(hck_space_atom);
			}
		}else{
			args.push_back(atom);
		}
	}
	space=new_sep(args);
}

//change hck in spatial part of B
void PASolver::change_hck(z3::expr &data,z3::expr &space,z3::expr_vector &ex_var_set){
	z3::expr_vector args(z3_ctx);
	for(int i=0;i<space.num_args();i++){
		z3::expr atom = space.arg(i);
		if(ishck(atom)){
			HeapChunk* hckdef = m_problem->getHeapChunk();
			z3::expr_vector atom_args(z3_ctx);
			for(int j=0;j<atom.num_args();j++){
				atom_args.push_back(atom.arg(j));
			}
			
			z3::expr_vector new_ex_set(z3_ctx);
			for(int j=0;j<hckdef->get_ex_size();j++){
				std::string ex_newvar_name = "[ex,"+std::to_string(ex_newvar_index)+"]";ex_newvar_index++;
		    	z3::expr ex_newvar = z3_ctx.constant(z3_ctx.str_symbol(ex_newvar_name.c_str()),z3_ctx.int_sort());
		    	new_ex_set.push_back(ex_newvar);
			}
			
			//change data
			z3::expr hck_data = hckdef->get_data();
			par2arg(hck_data,hckdef->get_pars(),atom_args,new_ex_set);
			data = data && hck_data;
			
			//change spatial atoms
			z3::expr_vector hck_space = hckdef->get_space();
			for(int j=0;j<hck_space.size();j++){
				z3::expr hck_space_atom = hck_space[j];
				par2arg(hck_space_atom,hckdef->get_pars(),atom_args,new_ex_set);
				args.push_back(hck_space_atom);
			}
			
			//merge ex_var_set
			ex_var_set=merge(ex_var_set,new_ex_set);
		}else{
			args.push_back(atom);
		}
	}
	space=new_sep(args);
}

void PASolver::pre_match(z3::expr absA, z3::expr &spaceA, z3::expr dataB, z3::expr &spaceB, z3::expr_vector &ex_var_set, z3::expr_vector &pi_ex_set){
	bool remA[spaceA.num_args()];
	bool remB[spaceB.num_args()];
	for(int i = 0;i<spaceA.num_args();i++){
		remA[i] = true;
	} 
	for(int i = 0;i<spaceB.num_args();i++){
		remB[i] = true;
	}
	for(int i=0;i<spaceB.num_args();i++){
		z3::expr atomB = spaceB.arg(i);
		z3::expr beg=head(atomB);
		z3::expr_vector tmpargs(z3_ctx);
		int startA=-1;
		for(int j=0;j<spaceA.num_args();j++){
			if(beg.to_string() == head(atomB).to_string()) startA++;
			z3::expr atomA = spaceA.arg(j);
			if(rel.get_relation(beg,head(atomA))==1){//beg == head(atomA)
				tmpargs.push_back(atomA);
				if(rel.get_relation(tail(atomB),tail(atomA))==1){//tail(atomB) == tail(atomA)
//std::cout<<"prematch :----------\n";
//tmpargs matches atomB 
//for(int k=0;k<spaceA.num_args();k++){
//	if(find_index(tmpargs,spaceA.arg(k))!=-1){
//std::cout<<spaceA.arg(k)<<"*";
//	}
//}
//std::cout<<std::endl;
//std::cout<<atomB<<std::endl;
					if(atomB.decl().name().str() != "blk" && !match_atom(absA, new_sep(tmpargs), dataB, atomB, ex_var_set, pi_ex_set)){
//std::cout<<"prematch is false:----------\n";
						throw -1;
					}
//std::cout<<"prematch is true:----------\n";
					//tmpargs matches atomB 
					for(int k=0;k<spaceA.num_args();k++){
						if(find_index(tmpargs,spaceA.arg(k))!=-1){
							remA[k]=false;
						}
					}
					remB[i]=false;
					break;
				}else if(rel.get_relation(tail(atomB),tail(atomA))==4 || rel.get_relation(tail(atomB),tail(atomA))==5){//tail(atomB)>tail(atomA) or tail(atomB)>=tail(atomA)
					beg=tail(atomA);
					j=-1;
				}else{//a_startA*... can't match atomB
					z3::expr_vector empargs(z3_ctx);
					tmpargs = empargs;
					beg = head(atomB);
					j = startA;
				}
			}	
		}
	}
	z3::expr_vector newargsA(z3_ctx);
	for(int i=0;i<spaceA.num_args();i++){
		if(remA[i]) newargsA.push_back(spaceA.arg(i));
	}
	z3::expr_vector newargsB(z3_ctx);
	for(int i=0;i<spaceB.num_args();i++){
		if(remB[i]) newargsB.push_back(spaceB.arg(i));
	}
	spaceA=new_sep(newargsA);
	spaceB=new_sep(newargsB);
}


void PASolver::pi_remove_ex(z3::expr& pi){
	if(pi.is_const()){//normal variable
		return;
	}
	if(pi.arg(0).is_var()){//existential variable
		pi=z3_ctx.bool_val(true);
	}else{
		z3::expr_vector tmp_pars(z3_ctx);
		for(int i=0;i<pi.num_args();i++){
			z3::expr tmp=pi.arg(i);
			pi_remove_ex(tmp);
			tmp_pars.push_back(tmp);
		}
		pi=pi.decl()(tmp_pars);
	}
}

void PASolver::delete_ele(z3::expr_vector &ex_var_set,z3::expr ex_var){
	z3::expr_vector new_ex_var_set(z3_ctx);
	for(int i=0;i<ex_var_set.size();i++){
		if(ex_var_set[i].to_string()!=ex_var.to_string()) new_ex_var_set.push_back(ex_var_set[i]);
	}
	ex_var_set=new_ex_var_set;
}

bool PASolver::add_exp2pi_ex_set_and_check(z3::expr_vector &pi_ex_set,z3::expr exp,z3::expr absC,z3::expr dataD,z3::expr_vector ex_var_set){
	z3::expr_vector new_pi_ex_set(z3_ctx);
	for(int i=0;i<pi_ex_set.size();i++){
		z3::expr newexp=pi_ex_set[i] && exp;
		new_pi_ex_set.push_back(newexp); 
		z3_sol.reset();
		z3::expr con(z3_ctx);
		if(ex_var_set.size()!=0){
			con=exists(ex_var_set,dataD);
		}else{
			con=dataD;		
		}
		z3_sol.add(absC && newexp && (!con));
		if(z3_sol.check()==z3::sat) return false;
	}
	pi_ex_set=new_pi_ex_set;
	return true;
}

bool PASolver::match_atom(z3::expr absC, z3::expr spaceC, z3::expr dataD, z3::expr d1, z3::expr_vector &ex_var_set, z3::expr_vector &pi_ex_set){	
	if(d1.decl().name().str() == "pto"){//d1 is pto
		z3::expr t = d1.arg(0);
		z3::expr sref = d1.arg(1);
    	z3::expr u = sref.arg(0);
		if(spaceC.num_args()==1 && spaceC.arg(0).decl().name().str() == "pto"){//spaceC is pto
			z3::expr atomC = spaceC.arg(0);
			z3::expr t1 = atomC.arg(0);
			z3::expr sref1 = atomC.arg(1);
    		z3::expr u1 = sref1.arg(0);
			if(find_index(ex_var_set,u)==-1){
				z3::expr con = (t == t1) && (u == u1);
				z3_sol.reset();
				z3_sol.add(absC && (!con));
				if(z3_sol.check() == z3::unsat) return true;
			}else{//u is existential variable
				delete_ele(ex_var_set,u);
				z3::expr exp = u==u1;
				if(add_exp2pi_ex_set_and_check(pi_ex_set,exp,absC,dataD,ex_var_set)) return true;
			}
		}else if(spaceC.num_args()==1 && m_problem->getPredicate()!=nullptr){
			z3::expr c1 = spaceC.arg(0);
    		if(isP(c1)){//spaceC is P
    			Predicate_SLAH *pdef = dynamic_cast<Predicate_SLAH*>(m_problem->getPredicate()); 
    			z3::expr hck=pdef->get_hck();
    			HeapChunk* hckdef=m_problem->getHeapChunk();
    			//ishck(hck);
    			z3::expr pi=hckdef->get_data();
    			pi_remove_ex(pi);
    			
    			z3::expr_vector args(z3_ctx);
    			for(int i=0;i<c1.num_args();i++){
    				args.push_back(c1.arg(i));
				}
    			par2arg(pi,hckdef->get_pars(),args);
    			if(find_index(ex_var_set,u)==-1){
    				z3::expr con = (t == head(c1)) && ((t+1) == tail(c1)) && (u == 1) && pi;
	    			z3_sol.reset();
	    			z3_sol.add(absC && (!con));
	    			if(z3_sol.check() == z3::unsat) return true;
    			}else{//u is existential variable
    				z3::expr con = (t == head(c1)) && ((t+1) == tail(c1)) && pi;
	    			z3_sol.reset();
	    			z3_sol.add(absC && (!con));
	    			if(z3_sol.check() == z3::unsat){
	    				delete_ele(ex_var_set,u);
						z3::expr exp = u==1;
						add_exp2pi_ex_set_and_check(pi_ex_set,exp,absC,dataD,ex_var_set);
	    				return true;
					}
				}
    			
			}
		}
	}else if(d1.decl().name().str() == "blk"){//d1 is blk
		if(spaceC.num_args()>0){
			z3::expr atom1 = spaceC.arg(0);
			z3::expr con = head(d1) == head(atom1);
			for(int i = 0;i<spaceC.num_args()-1;i++){
				atom1 = spaceC.arg(i);
				z3::expr atom2 = spaceC.arg(i+1);
				con = con && (tail(atom1) == head(atom2));
			}
			z3_sol.reset();
			z3_sol.add(absC && (! con));
			if(z3_sol.check() == z3::unsat) return true;
		}
	}else{//d1 is P
		if(isP(d1)){
			return match_P(absC, spaceC, dataD, d1,ex_var_set);
		}
	}
	return false;
}

z3::expr PASolver::getEUB(z3::expr c1, z3::expr d1){
	z3::expr con=z3_ctx.bool_val(true);
	Predicate_SLAH *d1_pdef = dynamic_cast<Predicate_SLAH*>(m_problem->getPredicate()); 
	HeapChunk* hckdef = m_problem->getHeapChunk();
	int zsize=d1_pdef->get_z_size();
	for(int i=1;i<=zsize;i++){
		con = con && (c1.arg(i)==d1.arg(i)) && (c1.arg(zsize+1+i)==d1.arg(zsize+1+i));
	}
	for(int i=2*(zsize+1);i<c1.num_args();i++){
		bool isbound=false;
		std::vector<int> vset=hckdef->get_v_index_set();
		for(int j=0;j<vset.size();j++){
			if(i==vset[j]){
				isbound=true;
				break;
			} 
		}
		if(isbound){
			if(i==hckdef->get_v_index()){
				int min_bound=hckdef->get_min_bound();
				z3::expr sz=tail(c1)-head(c1);
				std::string z = "[z]";
				z3::expr var_z = z3_ctx.constant(z3_ctx.str_symbol(z.c_str()),z3_ctx.int_sort());
				z3::expr con_mub1=c1.arg(i) == min_bound && var_z==min_bound && mod(sz,min_bound) == 0;
				z3::expr con_mub21=c1.arg(i) > min_bound && var_z >= min_bound && var_z<=c1.arg(i) && (min_bound <= sz-var_z || sz==var_z);
				z3::expr con_mub22=var_z+1 > c1.arg(i) || (sz<=var_z+min_bound && sz!=var_z+1);
				z3::expr con_mub2 = con_mub21 && con_mub22;
				for(int mub_i = 2;mub_i<=min_bound;mub_i++){
					z3::expr con_mub23=var_z+mub_i > c1.arg(i) || sz!=var_z+mub_i;
					con_mub2 = con_mub2 && con_mub23;
				}
				
				z3::expr mub_body=(con_mub1 || con_mub2) && var_z<=d1.arg(i);
				z3::expr con_mub=exists(var_z,mub_body);
				con = con && con_mub;
			}else{
				con=con && (c1.arg(i) <= d1.arg(i));
			}
		}else{
			con=con && (c1.arg(i) == d1.arg(i));
		}
	}
//cout<<"absC*******"<<absC<<endl;
//cout<<"here*******"<<con<<endl;
	z3::expr sz1=tail(c1)-head(c1);
	z3::expr sz2=tail(d1)-head(d1);
	con = con || (sz1 == 0 && sz2 == 0);//hls may be emp from prematch
	return con;
}

bool PASolver::match_P(z3::expr absC, z3::expr spaceC, z3::expr dataD, z3::expr d1, z3::expr_vector ex_var_set){
//std::cout<<"-----------------match_P--------------------"<<std::endl;
//std::cout<<"absC: "<<absC<<std::endl;
//std::cout<<"spaceC: "<<spaceC<<std::endl;
//std::cout<<"d1: "<<d1<<std::endl;
	Predicate_SLAH *d1_pdef = dynamic_cast<Predicate_SLAH*>(m_problem->getPredicate()); 
	int zsize=d1_pdef->get_z_size();
	int vsize=d1_pdef->get_v_size();
	z3::func_decl p_fun = d1_pdef->get_fun();
	HeapChunk* hckdef = m_problem->getHeapChunk();
	int hck_exsize = hckdef->get_ex_size();
	
	z3::expr con=z3_ctx.bool_val(true);
	for(int i=0;i<zsize+1;i++){
		con=con && (d1.arg(i) == d1.arg(1+zsize+i)); 
	}
	z3_sol.reset();
	z3_sol.add(absC && (!con));
	if(spaceC.num_args()==0 && z3_sol.check() == z3::unsat){
//std::cout<<"-------match_P is true: left:emp right:P(x,x)---------"<<std::endl;
		 return true;//m===0 and x=y and \vec{z} = \vec{u}
	}
	else if(spaceC.num_args()>=1){//m>=1
		con = head(d1) == head(spaceC.arg(0));
		for(int i=0;i<spaceC.num_args()-1;i++){
			z3::expr atom1=spaceC.arg(i);
			z3::expr atom2=spaceC.arg(i+1);
			con = con && (tail(atom1) == head(atom2));
		}
		z3_sol.reset();
		z3_sol.add(absC && (!con));
		if(z3_sol.check()==z3::unsat){//m>=1 and head(spaceC)==head(d1) and consecutive
			z3::expr c1 = spaceC.arg(0);
			if(c1.decl().name().str()=="blk"){
//std::cout<<"-------match_P is false: left:blk*... ---------"<<std::endl;
				return false;//m>=1 and consecutive and c1 is blk
			}else if(isP(c1)){//m>=1 and consecutive and c1 is P
				Predicate_SLAH *c1_pdef = dynamic_cast<Predicate_SLAH*>(m_problem->getPredicate()); 
				if(spaceC.num_args()==1){//m==1 and consecutive and c1 is P
					con=getEUB(c1,d1);
//std::cout<<absC<<std::endl;
//std::cout<<con<<std::endl;
					z3_sol.reset();
					z3_sol.add(absC && (!con));
					if(z3_sol.check()==z3::unsat){
//std::cout<<"-------match_P is true: left:P(x,y) right:P(x,y)---------"<<std::endl;
						return true;
					}
//z3::model m = z3_sol.get_model();
//for (int i=0; i<m.num_consts(); i++) {
//    z3::func_decl x = m.get_const_decl(i);
//    z3::expr x_interp = m.get_const_interp(x);
//    cout  <<x.name() << ":" << x_interp<<endl;
//}
				}else{//m>1 and consecutive and c1 is P -> divide d1
					z3::expr_vector atomset1(z3_ctx);
					atomset1.push_back(c1);
					
					z3::expr_vector atomset2(z3_ctx);
					for(int i=1;i<spaceC.num_args();i++){
						atomset2.push_back(spaceC.arg(i));
					}
					
					z3::expr_vector args_div1(z3_ctx);
					for(int i=0;i<zsize+1;i++){//x,\vec{z}
						args_div1.push_back(d1.arg(i));
					}
					for(int i=(zsize+1);i<2*(zsize+1);i++){//y1,\vec{u}'
						args_div1.push_back(c1.arg(i));
					}
					for(int i=2*(zsize+1);i<d1.num_args();i++){//\vec{v}
						args_div1.push_back(d1.arg(i)); 
					} 
					z3::expr d1_div1=p_fun(args_div1);
					
					z3::expr_vector args_div2(z3_ctx);//y1,\vec{u}'
					for(int i=(zsize+1);i<2*(zsize+1);i++){
						args_div2.push_back(c1.arg(i));
					}
					for(int i=(zsize+1);i<d1.num_args();i++){//y,\vec{u},\vec{v}
						args_div2.push_back(d1.arg(i));
					}
					z3::expr d1_div2=p_fun(args_div2);
//std::cout<<"-----left:P*...,divide d1 to two match_P problems-----"<<std::endl;
					return match_P(absC,new_sep(atomset1),dataD,d1_div1,ex_var_set) && match_P(absC,new_sep(atomset2),dataD,d1_div2,ex_var_set);
				}
			}else if(c1.decl().name().str()=="pto"){//m>=1 and consecutive and c1 is pto
				z3::expr x1 = c1.arg(0);
				z3::expr sref1 = c1.arg(1);
    			z3::expr t1 = sref1.arg(0);
    			
    			std::vector<int> set1;//x1+t1=tail(cj)
				std::vector<int> set2;//x1+t1 is in cj and cj is P
    			z3::expr cm = spaceC.arg(spaceC.num_args()-1);
				set1.push_back(spaceC.num_args()-1);
				con = (x1 + t1) == tail(cm);
				if(isP(cm)){
					set2.push_back(spaceC.num_args()-1);
					con = con || ((head(cm) < (x1+t1)) && ((x1+t1) < tail(cm)));
				}
				for(int i=0;i<spaceC.num_args()-1;i++){
					z3::expr ci=spaceC.arg(i);
					if(spaceC.arg(i+1).decl().name().str() != "blk"){
						set1.push_back(i);
						con = con || ((x1 + t1) == tail(ci));
					}
					if(isP(ci)){
						set2.push_back(i);
						con = con || ((head(ci) < (x1+t1)) && ((x1+t1) < tail(ci)));
					}
				}
				z3_sol.reset();
				z3_sol.add(absC && (!con));
				if(z3_sol.check()==z3::unsat){
					for(int i=0;i<set1.size();i++){//x1+t1==tail(ci)
						z3::expr ci=spaceC.arg(set1[i]);
						con=absC && ((x1+t1) == tail(ci));
						z3_sol.reset();
						z3_sol.add(con);
						if(z3_sol.check()==z3::sat){//absC && ((x1+t1) == tail(ci)) is sat
//std::cout<<"x1+t1=tail(c"<<set1[i]<<")"<<std::endl;
							z3::expr_vector w_newvar_set(z3_ctx);
							for(int j=0;j<hck_exsize;j++){
								std::string w_newvar_name = "[w,"+std::to_string(w_newvar_index)+"]";w_newvar_index++;
		    					z3::expr w_newvar = z3_ctx.constant(z3_ctx.str_symbol(w_newvar_name.c_str()),z3_ctx.int_sort());
		    					w_newvar_set.push_back(w_newvar);
							}
							
							if(zsize==0){//P(x;y;\vec{v}) hck(x,y,\vec{v})
//std::cout<<"----match_P c1 to spaceC.arg"<<set1[i]<<" part1-----"<<std::endl;
								z3::expr sub1_dataA=con;//absC && ((x1+t1) == tail(ci))
								z3::expr_vector sub1_args(z3_ctx);
								sub1_args.push_back(d1.arg(0));//x
								sub1_args.push_back(tail(ci));//tail(ci)
								for(int k=2;k<d1.num_args();k++) sub1_args.push_back(d1.arg(k));//\vec{v}
								
								z3::expr_vector sub1_atomsetA(z3_ctx);
								for(int k=0;k<=set1[i];k++) sub1_atomsetA.push_back(spaceC.arg(k));//c1*...*cj
								
								z3::expr defpi=hckdef->get_data();
								par2arg(defpi,hckdef->get_pars(),sub1_args,w_newvar_set);
								z3::expr sub1_dataB=dataD && defpi;
								
								z3::expr_vector sub1_atomsetB(z3_ctx);
								for(int k=0;k<hckdef->get_space().size();k++){
									z3::expr tmp = hckdef->get_space()[k];
									par2arg(tmp,hckdef->get_pars(),sub1_args,w_newvar_set);
									sub1_atomsetB.push_back(tmp);
								}
								z3::expr_vector pi_ex_set(z3_ctx);
								z3::expr pi_ex=z3_ctx.bool_val(true);
								pi_ex_set.push_back(pi_ex);
								z3::expr_vector new_ex_var_set=merge(ex_var_set,w_newvar_set);
								if(check_entl_get_relations(sub1_dataA,new_sep(sub1_atomsetA),sub1_dataB,new_sep(sub1_atomsetB),new_ex_var_set,pi_ex_set)==z3::sat) return false;
							
//std::cout<<"part2"<<std::endl;
								//part2
								for(int j=0;j<pi_ex_set.size();j++){
									z3::expr sub2_dataA=con && pi_ex_set[j];
									z3::expr_vector sub2_atomsetA(z3_ctx);
									for(int k=set1[i]+1;k<spaceC.num_args();k++)sub2_atomsetA.push_back(spaceC.arg(k));//c_{j+1}*...*cm
									z3::expr sub2_dataB = sub1_dataB;
									
									z3::expr_vector sub2_args(z3_ctx);
									sub2_args.push_back(tail(ci));//tail(ci)
									for(int k=1;k<d1.num_args();k++) sub2_args.push_back(d1.arg(k));//y,\vec{v}
									z3::expr sub2_atomB = p_fun(sub2_args);
//std::cout<<"part2dataA: "<<sub2_dataA<<std::endl;
//std::cout<<"part2spaceA: "<<new_sep(sub2_atomsetA)<<std::endl;
//std::cout<<"part2dataB: "<<sub2_dataB<<std::endl;
//std::cout<<"part2atomB: "<<sub2_atomB<<std::endl;
//std::cout<<"----match_P c1 to spaceC.arg"<<set1[i]<<" part2-----"<<std::endl;
									if(!match_P(sub2_dataA,new_sep(sub2_atomsetA),sub2_dataB,sub2_atomB,ex_var_set)) return false;
								}
							}else{//P(x,\vec{z};y,\vec{u};\vec{v}) hck(x,\vec{z},w,y,\vec{u},\vec{v}) or hck(x,\vec{z};y,\vec{u};\vec{v})
								//zsize>0
								std::vector<path> transitionset=hckdef->get_transition_set();
								for(int j=0;j<transitionset.size();j++){
//std::cout<<"transition "<<j<<std::endl;
									z3::expr con_tran = con;//absC && ((x1+t1) == tail(ci))
									std::vector<int> source = transitionset[j].source;
									std::vector<int> dest = transitionset[j].dest;
									for(int k=1;k<zsize+1;k++){
										con_tran = con_tran && (d1.arg(k) == source[k-1]);
									}
									
									z3_sol.reset();
									z3_sol.add(con_tran);
									if(z3_sol.check()==z3::sat){//con && \vec{z} = source is satisfiable.con is different when wsize==1 and wsize==0
										//divide d1 to two part
										//part1
//std::cout<<"part1"<<std::endl;
										z3::expr sub1_dataA=con_tran;
										z3::expr_vector sub1_args(z3_ctx);
										for(int k=0;k<zsize+1;k++){//x,\vec{z}
											sub1_args.push_back(d1.arg(k));
										}
										sub1_args.push_back(tail(ci));//tail(ci)
										z3::expr_vector z_newvar_set(z3_ctx);
										for(int k=0;k<dest.size();k++){//\vec{z}' is \vec{q_2} directly
											sub1_args.push_back(z3_ctx.int_val(dest[k]));
										}
										for(int k=2*(zsize+1);k<d1.num_args();k++){//\vec{v}
											sub1_args.push_back(d1.arg(k));
										}
										
										z3::expr_vector sub1_atomsetA(z3_ctx);
										for(int k=0;k<=set1[i];k++){
											sub1_atomsetA.push_back(spaceC.arg(k));
										}
										
										z3::expr defpi=hckdef->get_data();
										par2arg(defpi,hckdef->get_pars(),sub1_args,w_newvar_set);
										z3::expr sub1_dataB=dataD && defpi;
										
										z3::expr_vector sub1_atomsetB(z3_ctx);
										for(int k=0;k<hckdef->get_space().size();k++){
											z3::expr tmp = hckdef->get_space()[k];
											par2arg(tmp,hckdef->get_pars(),sub1_args,w_newvar_set);
											sub1_atomsetB.push_back(tmp);
										}
										z3::expr_vector pi_ex_set(z3_ctx);
										z3::expr pi_ex=z3_ctx.bool_val(true);
										pi_ex_set.push_back(pi_ex);
										z3::expr_vector new_ex_var_set=merge(ex_var_set,w_newvar_set);
//std::cout<<"----match_P c1 to spaceC.arg"<<set1[i]<<" transition"<<j<<" part1-----"<<std::endl;
										if(check_entl_get_relations(sub1_dataA,new_sep(sub1_atomsetA),sub1_dataB,new_sep(sub1_atomsetB),new_ex_var_set,pi_ex_set)==z3::sat) return false;						
//std::cout<<"part2"<<std::endl;
										
										//part2
										for(int l=0;l<pi_ex_set.size();l++){
											z3::expr sub2_dataA=con_tran && pi_ex_set[l];
											z3::expr_vector sub2_atomsetA(z3_ctx);
											for(int k=set1[i]+1;k<spaceC.num_args();k++){
												sub2_atomsetA.push_back(spaceC.arg(k));
											}
											z3::expr sub2_dataB = sub1_dataB;
											
											z3::expr_vector sub2_args(z3_ctx);
											sub2_args.push_back(tail(ci));//tail(ci)
											for(int k=0;k<dest.size();k++){//\vec{z}' is \vec{q_2} directly
												sub2_args.push_back(z3_ctx.int_val(dest[k]));
											}
											for(int k=zsize+1;k<d1.num_args();k++){//y,\vec{u},\vec{v}
												sub2_args.push_back(d1.arg(k));
											}
											z3::expr sub2_atomB = p_fun(sub2_args);
//std::cout<<"part2dataA: "<<sub2_dataA<<std::endl;
//std::cout<<"part2spaceA: "<<new_sep(sub2_atomsetA)<<std::endl;
//std::cout<<"part2dataB: "<<sub2_dataB<<std::endl;
//std::cout<<"part2atomB: "<<sub2_atomB<<std::endl;
//std::cout<<"----match_P c1 to spaceC.arg"<<set1[i]<<" transition"<<j<<" part2-----"<<std::endl;
											if(!match_P(sub2_dataA,new_sep(sub2_atomsetA),sub2_dataB,sub2_atomB,ex_var_set)) return false;
										}
										
									}
								}
							}	
						}
					}
					for(int i=0;i<set2.size();i++){//ci is P and head(ci)<x1+t1<tail(ci)
						z3::expr ci=spaceC.arg(set2[i]);
						con=absC && (head(ci) < (x1+t1)) && ((x1+t1) < tail(ci));
						z3_sol.reset();
						z3_sol.add(con);
						if(z3_sol.check()==z3::sat){
							//two case
							//the first case:ci->P(x,...;x1+t1,...;...)*P(x1+t1,...;y,...;...)
							z3::expr case1_dataA = absC;
							z3::expr_vector case1_atomsetA(z3_ctx);
							for(int j=0;j<set2[i];j++){//c_1*...*c_{i-1}
								case1_atomsetA.push_back(spaceC.arg(j));
							}
							
							z3::expr_vector ci_div1_args(z3_ctx);
							for(int j=0;j<zsize+1;j++){//xi,\vec{z}'
								ci_div1_args.push_back(ci.arg(j));
							}
							ci_div1_args.push_back(x1+t1);
							z3::expr_vector z_newvar_set(z3_ctx);
							for(int j=0;j<zsize;j++){
								std::string z_newvar_name = "[z,"+std::to_string(z_newvar_index)+"]";z_newvar_index++;
    							z3::expr z_newvar = z3_ctx.constant(z3_ctx.str_symbol(z_newvar_name.c_str()),z3_ctx.int_sort());
    							ci_div1_args.push_back(z_newvar);//\vec{z}''
    							z_newvar_set.push_back(z_newvar);
							}
							for(int j=2*(zsize+1);j<ci.num_args();j++){//\vec{v}'
								ci_div1_args.push_back(ci.arg(j));
							}
							z3::expr ci_div1=p_fun(ci_div1_args);
							
							z3::expr_vector ci_div2_args(z3_ctx);
							ci_div2_args.push_back(x1+t1);
							for(int j=0;j<z_newvar_set.size();j++){//\vec{z}''
								ci_div2_args.push_back(z_newvar_set[j]);
							}
							for(int j=zsize+1;j<ci.num_args();j++){//\yi,\vec{u}',\vec{v}'
								ci_div2_args.push_back(ci.arg(j));
							}
							z3::expr ci_div2=p_fun(ci_div2_args);
							
							case1_atomsetA.push_back(ci_div1);
							case1_atomsetA.push_back(ci_div2);
							
							for(int j=set2[i]+1;j<spaceC.num_args();j++){////c_{i+1}*...*c_m
								case1_atomsetA.push_back(spaceC.arg(j));
							}
							
							z3::expr case1_dataB = dataD;
							z3::expr case1_atomB = d1;
//std::cout<<"----match_P c1 to inside of spaceC.arg"<<set2[i]<<" case1-----"<<std::endl;
							if(!match_P(case1_dataA,new_sep(case1_atomsetA),case1_dataB,case1_atomB,ex_var_set)) return false;
							
							
							//the second case: ci->P(x,...;head(b1),...;...)*b1*...*bl*P(tail(bl),...;y,...;...) and x1+t1==tail(bj)
							for(int j=0;j<hckdef->get_space().size();j++){
								z3::expr_vector case2_ci_div1_args(z3_ctx);
								z3::expr_vector case2_ci_div2_args(z3_ctx);
								z3::expr_vector case2_ci_div3_args(z3_ctx);
								
								//case2_ci_div1_args
								for(int k=0;k<zsize+1;k++){//xi,\vec{z}'
									case2_ci_div1_args.push_back(ci.arg(k));
								}
								std::string w1_newvar_name = "[w1,"+std::to_string(z_newvar_index)+"]";
    							z3::expr w1_newvar = z3_ctx.constant(z3_ctx.str_symbol(w1_newvar_name.c_str()),z3_ctx.int_sort());
    							std::string w2_newvar_name = "[w2,"+std::to_string(z_newvar_index)+"]";
    							z3::expr w2_newvar = z3_ctx.constant(z3_ctx.str_symbol(w2_newvar_name.c_str()),z3_ctx.int_sort());
    							case2_ci_div1_args.push_back(w1_newvar);//w
    							
    							z3::expr_vector z1_newvar_set(z3_ctx);
    							z3::expr_vector z2_newvar_set(z3_ctx);
								for(int k=0;k<zsize;k++){
									std::string z1_newvar_name = "[z1,"+std::to_string(z_newvar_index)+"]";
									std::string z2_newvar_name = "[z2,"+std::to_string(z_newvar_index)+"]";
									z_newvar_index++;
    								z3::expr z1_newvar = z3_ctx.constant(z3_ctx.str_symbol(z1_newvar_name.c_str()),z3_ctx.int_sort());
    								z3::expr z2_newvar = z3_ctx.constant(z3_ctx.str_symbol(z2_newvar_name.c_str()),z3_ctx.int_sort());
    								case2_ci_div1_args.push_back(z1_newvar);//\vec{z}''
    								z1_newvar_set.push_back(z1_newvar);
    								z2_newvar_set.push_back(z2_newvar);
								}
								
								for(int k=2*(zsize+1);k<ci.num_args();k++){//\vec{v}'
									case2_ci_div1_args.push_back(ci.arg(k));
								}
								
								//case2_ci_div2_args
								case2_ci_div2_args.push_back(w1_newvar);//w1
								for(int k=0;k<z1_newvar_set.size();k++){//\vec{z}''
									case2_ci_div2_args.push_back(z1_newvar_set[k]);
								}
								z3::expr_vector new_ex_var_set(z3_ctx); 
								for(int k=0;k<hck_exsize;k++){//w
									std::string w_newvar_name = "[w,"+std::to_string(w_newvar_index)+"]";w_newvar_index++;
	    							z3::expr w_newvar = z3_ctx.constant(z3_ctx.str_symbol(w_newvar_name.c_str()),z3_ctx.int_sort());
									new_ex_var_set.push_back(w_newvar);
								}
    							case2_ci_div2_args.push_back(w2_newvar);//w2
    							for(int k=0;k<z2_newvar_set.size();k++){//vec{z}'''
									case2_ci_div2_args.push_back(z2_newvar_set[k]);
								}
								for(int k=2*(zsize+1);k<ci.num_args();k++){//\vec{v}'
									case2_ci_div2_args.push_back(ci.arg(k));
								}
    							
    							//case2_ci_div3_args
    							case2_ci_div3_args.push_back(w2_newvar);//w2
    							for(int k=0;k<z2_newvar_set.size();k++){//\vec{z}'''
									case2_ci_div3_args.push_back(z2_newvar_set[k]);
								}
								for(int k=zsize+1;k<ci.num_args();k++){//y,\vec{u}',\vec{v}'
									case2_ci_div3_args.push_back(ci.arg(k));
								}
    							
    							z3::expr pi=hckdef->get_data();
    							par2arg(pi,hckdef->get_pars(),case2_ci_div2_args,new_ex_var_set);
    							z3::expr_vector sigma(z3_ctx);
    							for(int k=0;k<hckdef->get_space().size();k++){
    								z3::expr bk = hckdef->get_space()[k];
    								par2arg(bk,hckdef->get_pars(),case2_ci_div2_args,new_ex_var_set);
    								sigma.push_back(bk);
								}
								z3::expr case2_dataA = absC && ((x1+t1) == tail(sigma[j])) && pi;
								
								
								z3::expr_vector case2_atomsetA(z3_ctx);
								for(int k=0;k<set2[j];k++){
									case2_atomsetA.push_back(spaceC.arg(k));
								}
								case2_atomsetA.push_back(p_fun(case2_ci_div1_args));
								for(int k=0;k<sigma.size();k++){
									case2_atomsetA.push_back(sigma[k]);
								}
								case2_atomsetA.push_back(p_fun(case2_ci_div3_args));
								for(int k=set2[j]+1;k<spaceC.num_args();k++){
									case2_atomsetA.push_back(spaceC.arg(k));
								}
								
								z3::expr case2_dataB = dataD;
								z3::expr case2_atomB = d1;
//std::cout<<"----match_P c1 to inside of spaceC.arg"<<set2[i]<<" case2-----"<<std::endl;
								if(!match_P(case2_dataA,new_sep(case2_atomsetA),case2_dataB,case2_atomB,ex_var_set)) return false;
							}
						}
					}
//std::cout<<"------match_P is true----------"<<std::endl;
					return true;
				}
			}
		}
	}
//std::cout<<"------match_P is false----------"<<std::endl;
	return false;
}

void PASolver::pre_P(z3::expr spaceA, z3::expr dataA, z3::expr spaceB, z3::expr dataB, z3::expr absA, z3::expr spaceAhls, z3::expr spaceBhls, z3::expr_vector &ex_var_set, z3::expr_vector &pi_ex_set) throw (int){
	z3::expr_vector args(z3_ctx);
	if(spaceA.num_args() != 0){
		//remove the last atom from spaceA
		args = get_space_args(spaceA);
		z3::expr atom = args.back();//the last atom of spaceA
		args.pop_back();
		spaceA = new_sep(args);
		if(isP(atom)){
			z3::expr source = head(atom);
        	z3::expr dest = tail(atom);
			
			//the first possibility(size = 0):P->emp
			if(rel.get_relation(source,dest) == 0 || rel.get_relation(source,dest)==1 || rel.get_relation(source,dest)==3){//z1 ? z2 或者 z1 = z2  或者 z1 <= z2 
				z3_sol.reset();
    			z3_sol.add(absA && source == dest);
    			if(rel.get_relation(source,dest)==1 || z3_sol.check() == z3::sat){
    				if(spaceA.num_args() == 0 && spaceB.num_args() == 0 && spaceAhls.num_args() != 0){//spaceB is empty.when spaceA is emp or P(x,x),the entl may be true
    					throw -1;
					}else{
						pre_P(spaceA, dataA && source == dest, spaceB, dataB, absA && source == dest, spaceAhls, spaceBhls, ex_var_set, pi_ex_set);
					}
    			
    			}
			}
			
    		
    		//the first possibility(size >= 1):P->P
    		if(rel.get_relation(source,dest) == 0 || rel.get_relation(source,dest) == 2|| rel.get_relation(source,dest) == 3){//z1 ? z2 或者 z1 < z2 或者 z1 <= z2 
    			z3_sol.reset();
    			z3_sol.add(absA && (dest - source) >= 1);
    			if(z3_sol.check() == z3::sat){////////////////////////////////////////////////////////////
    				if(spaceA.num_args() == 0 && spaceB.num_args() == 0){//spaceB is empty and spaceA is not empty
    					throw -1; 
					}else{
						z3::expr_vector argsA(z3_ctx);
						argsA = get_space_args(spaceAhls);
						argsA.push_back(atom);
						pre_P(spaceA, dataA && (dest - source) >= 1, spaceB, dataB, absA && (dest - source) >= 1, new_sep(argsA), spaceBhls, ex_var_set, pi_ex_set);
					}
    			}
			}
    		
		}else{//atoms that are not hls is directly added to spaceAhls
			z3::expr_vector argsA(z3_ctx);
			argsA = get_space_args(spaceAhls);
			argsA.push_back(atom);
			spaceAhls = new_sep(argsA);
			if(spaceA.num_args() == 0 && spaceB.num_args() == 0){//spaceB is empty and spaceA is not emoty
    			throw -1;
			}else{
				pre_P(spaceA, dataA, spaceB, dataB, absA, spaceAhls, spaceBhls, ex_var_set, pi_ex_set);
			}
		}
	}else if(spaceB.num_args() != 0){
		args = get_space_args(spaceB);
		z3::expr atom = args.back();
		args.pop_back();
		spaceB = new_sep(args);
		if(isP(atom)){
			z3::expr source = head(atom);
        	z3::expr dest = tail(atom);
			
			//1
			if(rel.get_relation(source,dest) == 0 || rel.get_relation(source,dest)==1|| rel.get_relation(source,dest)==3){//z1 ? z2 或者 z1 = z2或者 z1 <= z2
				z3_sol.reset();
    			z3_sol.add(absA && source == dest);
    			if(rel.get_relation(source,dest)==1||z3_sol.check() == z3::sat){////////////////////////////////////////////////////////////
    				if(spaceA.num_args() == 0 && spaceB.num_args() == 0){
    					if((spaceAhls.num_args() != 0 && spaceBhls.num_args() == 0)||(spaceAhls.num_args() == 0 && spaceBhls.num_args() != 0)){
    						throw -1;
						}
    					if(spaceAhls.num_args() != 0 && spaceBhls.num_args() != 0&&check_entl_get_relations(dataA && source == dest, spaceAhls, dataB, spaceBhls,ex_var_set,pi_ex_set) == z3::sat){
    						throw -1;
						}	
					}else{
						pre_P(spaceA, dataA && source == dest, spaceB, dataB, absA && source == dest, spaceAhls, spaceBhls, ex_var_set,pi_ex_set);
					}
    			}
			}
	    		
    		
    		//2
    		if(rel.get_relation(source,dest) == 0 || rel.get_relation(source,dest) == 2|| rel.get_relation(source,dest) == 3){//z1 ? z2 或者 z1 < z2 或者 z1 <= z2 
    			z3_sol.reset();
	    		z3_sol.add(absA && (dest - source) >= 1);
	    		if(z3_sol.check() == z3::sat){///////////////////////////////////////////////////////////////
					z3::expr newdataA = dataA && (dest - source) >= 1;
					z3::expr newabsA = absA && (dest - source) >= 1;
					z3::expr_vector argsB(z3_ctx);
					argsB = get_space_args(spaceBhls);
					argsB.push_back(atom);
					if(spaceA.num_args() == 0 && spaceB.num_args() == 0){
	    				if(spaceAhls.num_args() == 0 || check_entl_get_relations(newdataA, spaceAhls, dataB, new_sep(argsB),ex_var_set,pi_ex_set) == z3::sat){
	    					throw -1;
						}
	    			}else
						pre_P(spaceA, newdataA, spaceB, dataB, newabsA, spaceAhls, new_sep(argsB),ex_var_set,pi_ex_set);	
	    		}
			}
	    		
		}else{
			z3::expr_vector argsB(z3_ctx);
			argsB = get_space_args(spaceBhls);
			argsB.push_back(atom);
			spaceBhls = new_sep(argsB);
			if(spaceA.num_args() == 0 && spaceB.num_args() == 0){
				if(spaceAhls.num_args() == 0 || check_entl_get_relations(dataA, spaceAhls, dataB, spaceBhls,ex_var_set,pi_ex_set) == z3::sat){
					throw -1;
				}
			}else{
				pre_P(spaceA, dataA, spaceB, dataB, absA, spaceAhls, new_sep(argsB),ex_var_set,pi_ex_set);
			}
		}
	}
		
}

z3::check_result PASolver::check_entl_get_relations(z3::expr dataA, z3::expr spaceA, z3::expr dataB, z3::expr spaceB, z3::expr_vector &ex_var_set, z3::expr_vector &pi_ex_set){
std::cout<<"---------check_entl_get_relations------------------------"<<std::endl;
std::cout<<"spaceA:"<<spaceA<<std::endl;
std::cout<<"spaceB:"<<spaceB<<std::endl;
//clock_t start, end;
//start = clock();
    z3::expr absA = get_abstraction(dataA, spaceA);
   
    z3::expr_vector headsA = get_space_heads(spaceA);
    z3::expr_vector tailsA = get_space_tails(spaceA);
    z3::expr_vector tailsB = get_space_tails(spaceB);

	//adds:headsA,tailsA,tailsB without repitition
	z3::expr_vector adds(z3_ctx);
	for(int i = 0; i < tailsA.size(); i++){
		adds.push_back(tailsA[i]);
	}
	for(int i = 0; i < headsA.size(); i++){
		if(find_index(adds,headsA[i]) == -1) adds.push_back(headsA[i]);
	}
	for(int i = 0; i < tailsB.size(); i++){
		if(find_index(adds,tailsB[i]) == -1) adds.push_back(tailsB[i]);
	}

	
	//adds_eq:Equivalence class
	std::vector<z3::expr_vector> adds_eq;
	//adds_rep:the representation of each equivalence class
	z3::expr_vector adds_rep(z3_ctx);
	for(int i = 0; i < adds.size(); i++){
		bool addsuc = false;
		for(int j = 0;j < adds_rep.size(); j++){
			if(rel.get_relation(adds[i],adds[j])==1){
				adds_eq[j].push_back(adds[i]);
    			addsuc = true;
    			break;
			}else if(rel.get_relation(adds[i],adds[j])!=2 && rel.get_relation(adds[i],adds[j])!=4){//不是 < 或 >
				z3_sol.reset();
	    		z3_sol.add(absA && (!(adds[i] == adds_rep[j])));
	    		if(z3_sol.check() == z3::unsat){//////////////////////////////////////////////////////////////////
	    			adds_eq[j].push_back(adds[i]);
	    			addsuc = true;
	    			break;
				}
			}
				
		}
		//a new equivalent class
		if(!addsuc){
			z3::expr_vector new_eq(z3_ctx);
			new_eq.push_back(adds[i]);
			adds_rep.push_back(adds[i]);
			adds_eq.push_back(new_eq);
		}
	}

	//rel_sub：子蕴涵问题中需要排序的地址已知的关系 
	Relation rel_sub;rel_sub.init(adds_rep); 
	for(int i = 0; i < adds_rep.size(); i++){
		for(int j = 0; j < adds_rep.size(); j++){
			if(rel.get_relation(adds_rep[i],adds_rep[j]) == 2){//<
				rel_sub.set_relation(adds_rep[i],adds_rep[j],2);
			}else if(rel.get_relation(adds_rep[i],adds_rep[j]) == 3){//最开始是<=，经过hls改写后是可能变成<的 
				z3_sol.reset();
    			z3_sol.add(absA && (!(adds_rep[i] < adds_rep[j])));
    			if(z3_sol.check() == z3::unsat){
    				rel_sub.set_relation(adds_rep[i],adds_rep[j],2);
				}else{
					rel_sub.set_relation(adds_rep[i],adds_rep[j],3);
				}
			}else if(rel.get_relation(adds_rep[i],adds_rep[j]) == 0){//?
				z3_sol.reset();
    			z3_sol.add(absA && (!(adds_rep[i] < adds_rep[j])));
    			if(z3_sol.check() == z3::unsat){
    				rel_sub.set_relation(adds_rep[i],adds_rep[j],2);
				}else{
					z3_sol.reset();
    				z3_sol.add(absA && (!(adds_rep[i] <= adds_rep[j])));
    				if(z3_sol.check() == z3::unsat)
						rel_sub.set_relation(adds_rep[i],adds_rep[j],3);
				}
			}
		}
	}
//rel.print();
//system("read REPLY");	
	
//end = clock();
//std::cout <<"get_relations:"<< (double)(end - start)/(double)CLOCKS_PER_SEC <<std::endl;
	
	
	return check_entl_get_sorts(adds_rep, rel_sub, absA, adds_eq, dataA, spaceA, dataB, spaceB,ex_var_set,pi_ex_set);
}

//relations<
//relations2<=
z3::check_result PASolver::check_entl_get_sorts(z3::expr_vector &adds_rep, Relation &rel_sub, z3::expr &absA, std::vector<z3::expr_vector> &adds_eq,z3::expr &dataA, z3::expr &spaceA, z3::expr &dataB, z3::expr &spaceB, z3::expr_vector &ex_var_set, z3::expr_vector &pi_ex_set){
	z3::expr_vector headsA = get_space_heads(spaceA);
    z3::expr_vector tailsA = get_space_tails(spaceA);
    z3::expr_vector tailsB = get_space_tails(spaceB);
	
	//flag:mark whether the eq_class in adds can be added
	std::vector<int> flag;
	for(int i=0;i<adds_rep.size();i++){
		flag.push_back(1);
	}
	for(int i = 0;i<adds_rep.size();i++){
		if(rel_sub.get_degree(adds_rep[i])>0) flag[i] = 0;
	} 
	
	//sort_set_tmp:set of incomplete sorts; sorts_tmp_pf:set of pure formulas coresponding to sorts
	std::vector<std::vector<z3::expr_vector>> sort_set_tmp;//由sort组成，每个sort由z3::expr_vector组成，在一个z3::expr_vector中地址的相等 
	std::vector<z3::expr> sort_set_tmp_pf;
	//initialize sorts_tmp and sorts_tmp_pf
	//sort_set_tmp:     {  {{x1}}  ,  {{y1}}  ,  {{z1}}  }
	//sort_set__tmp_pf: {   true   ,   true   ,  true    }
	for(int i = 0; i<adds_rep.size(); i++){ 
		if(flag[i] == 1){
			std::vector<z3::expr_vector> sort;
			z3::expr_vector eq_set(z3_ctx);
    		eq_set.push_back(adds_rep[i]);//{x1}
			sort.push_back(eq_set);//{{x1}}
			sort_set_tmp.push_back(sort);
			
			z3::expr sort_pf= z3_ctx.bool_val(true);
			sort_set_tmp_pf.push_back(sort_pf);
		}
	}
	
	while(!sort_set_tmp.empty()){
		//pop a sort_tmp from sort_set_tmp
		std::vector<z3::expr_vector> sort_tmp;
		sort_tmp = sort_set_tmp.back(); sort_set_tmp.pop_back();
		z3::expr sort_tmp_pf = z3_ctx.bool_val(true);
		sort_tmp_pf = sort_set_tmp_pf.back(); sort_set_tmp_pf.pop_back();
		
		//get flag based on sort_tmp
		std::vector<int> flag_tmp; 
		for(int i=0;i<adds_rep.size();i++){
			flag_tmp.push_back(1);
		}
		for(int i = 0; i < adds_rep.size(); i++){
			z3::expr add = adds_rep[i];
			if(get_eq_class(sort_tmp,add).size() ==0){//add不在这个sort_tmp中
				for(int j =0;j<adds_rep.size();j++){
					if(rel.get_relation(add,adds_rep[j]) == 2|| rel.get_relation(add,adds_rep[j]) == 3)
						flag_tmp[j] = 0;
				}
			}else{//add已经在这个sort_tmp中 
				flag_tmp[i] = 0;
			}
		}
		
		//add adds[i] into the sort_tmp
		for(int i = 0; i<adds_rep.size(); i++){
			if(flag_tmp[i] == 1){
				std::vector<z3::expr_vector> sort_tmp_new1;//=:merge eq_classes
				std::vector<z3::expr_vector> sort_tmp_new2;//<
				for(int j = 0; j<sort_tmp.size()-1;j++){
					sort_tmp_new1.push_back(sort_tmp[j]);
					sort_tmp_new2.push_back(sort_tmp[j]);
				}
				
				//last_add:merge the last eq_class of sort_tmp and adds[i]
				z3::expr_vector last_add(z3_ctx);
				for(int j = 0; j<sort_tmp.back().size();j++){
					last_add.push_back(sort_tmp.back()[j]);
				}
				last_add.push_back(adds_rep[i]);
				sort_tmp_new1.push_back(last_add);
				
				sort_tmp_new2.push_back(sort_tmp.back());
				z3::expr_vector eq_class(z3_ctx);
    			eq_class.push_back(adds_rep[i]);
				sort_tmp_new2.push_back(eq_class);
				
				
				bool is_less = false;//check sort_tmp.back()<adds[i] from relations<
				for(int k = 0;k<sort_tmp.back().size();k++){
					if(rel_sub.get_relation(sort_tmp.back()[k],adds_rep[i])==2){
						is_less = true;
						break;
					}	
				}
				
				//=
				if(!is_less){
					if(!is_in_sortset(sort_set_tmp,sort_tmp_new1)){//{x1,t1} and {t1,x1}, avoid duplication
						z3::expr sort_tmp_pf_new1 = sort_tmp_pf && (sort_tmp.back()[0] == adds_rep[i]);
						z3::expr con = absA && sort_tmp_pf_new1;
						//avoid {x1}{y1}.It should be {x1 x2-1}
						for(int j = 0;j<adds_rep.size();j++){//把当前添加的地址<=所有不在这个sort中的add这个条件也加上 
							z3::expr add=adds_rep[j];
							if(get_eq_class(sort_tmp_new1,add).size() == 0){
								con = con && (adds_rep[i] <= adds_rep[j]);
							}
						}
						
						z3_sol.reset();
						z3_sol.add(con);
						if(z3_sol.check() == z3::sat){/////////////////////////////////////////////////
							if(get_size(sort_tmp_new1) == adds_rep.size()){//sort is complete
								//get complete eq_class sort 
								std::vector<z3::expr_vector> sort;
								for(int j = 0;j<sort_tmp_new1.size();j++){
									z3::expr_vector eq_class(z3_ctx);
									for(int k = 0;k<sort_tmp_new1[j].size();k++){
										z3::expr sub_add = sort_tmp_new1[j][k];
										z3::expr_vector sub_eq_class = get_eq_class(adds_eq,sub_add);
										for(int l = 0;l<sub_eq_class.size();l++){
											eq_class.push_back(sub_eq_class[l]);
										}
									}
									sort.push_back(eq_class);
								}	
for(int j=0;j<sort.size();j++){
std::cout<<sort[j];
}
std::cout<<std::endl;
//system("read REPLY");
								if(!check_sort(sort, headsA, tailsA, tailsB)){
									std::cout<<"apply the sort, the entl is false"<<std::endl;
									return z3::sat;
								} 
								//check_sorted_entl 
								z3::expr dataA_sorted = absA && sort_tmp_pf_new1;
								z3::expr spaceA_sorted = spaceA;
								z3::expr spaceB_sorted = spaceB;
								//sort atoms according to the sort
								sort_space(sort,spaceA_sorted,spaceB_sorted,tailsA,tailsB);
								z3::expr_vector tmp_ex_var_set=ex_var_set;
								z3::expr_vector tmp_pi_ex_set=pi_ex_set;
								if(!check_sorted_entl(sort,dataA_sorted,spaceA_sorted,dataB,spaceB_sorted,tmp_ex_var_set,tmp_pi_ex_set)){
									std::cout<<"apply the sort, the entl is false"<<std::endl;
									return z3::sat;
								}			
							}else{//sort is incomplete，为了防止{x1,t1} and {t1,x1}这种重复，加入到前面比较好 
								std::vector<std::vector<z3::expr_vector>>::iterator begin = sort_set_tmp.begin();
								std::vector<z3::expr>::iterator begin_pf = sort_set_tmp_pf.begin();
								sort_set_tmp.insert(begin,sort_tmp_new1);
								sort_set_tmp_pf.insert(begin_pf,sort_tmp_pf_new1);
							}
						}
					}
				}

				
				//<
				if(!is_in_sortset(sort_set_tmp,sort_tmp_new2)){
					z3::expr sort_tmp_pf_new2 = sort_tmp_pf && (sort_tmp.back()[0] < adds_rep[i]);
					z3::expr con = absA && sort_tmp_pf_new2;
					for(int j = 0;j<adds_rep.size();j++){
						z3::expr add=adds_rep[j];
						if(get_eq_class(sort_tmp_new2,add).size() == 0){
							con = con && (adds_rep[i] <= adds_rep[j]);
						}
					}
					z3_sol.reset();
					z3_sol.add(con);
					if(z3_sol.check() == z3::sat){//////////////////////////////////////////////////////////////
						if(get_size(sort_tmp_new2) == adds_rep.size()){
							//get complete eq_class sort
							std::vector<z3::expr_vector> sort;
							for(int j = 0;j<sort_tmp_new2.size();j++){
								z3::expr_vector eq_class(z3_ctx);
								for(int k = 0;k<sort_tmp_new2[j].size();k++){
									z3::expr sub_add = sort_tmp_new2[j][k];
									z3::expr_vector sub_eq_class = get_eq_class(adds_eq,sub_add);
									for(int l = 0;l<sub_eq_class.size();l++){
										eq_class.push_back(sub_eq_class[l]);
									}
								}
								sort.push_back(eq_class);
							}

for(int j=0;j<sort.size();j++){
std::cout<<sort[j];
}
std::cout<<std::endl;
//system("read REPLY");
							if(!check_sort(sort, headsA, tailsA, tailsB)){
								std::cout<<"apply the sort, the entl is false"<<std::endl;
								return z3::sat;
							}
							//check_sorted_entl 
							z3::expr dataA_sorted = absA && sort_tmp_pf_new2;
							z3::expr spaceA_sorted = spaceA;
							z3::expr spaceB_sorted = spaceB;
							//sort atoms according to the sort
							sort_space(sort,spaceA_sorted,spaceB_sorted,tailsA,tailsB);
							z3::expr_vector tmp_ex_var_set=ex_var_set;
							z3::expr_vector tmp_pi_ex_set=pi_ex_set;
							if(!check_sorted_entl(sort,dataA_sorted,spaceA_sorted,dataB,spaceB_sorted,tmp_ex_var_set,tmp_pi_ex_set)){
								std::cout<<"apply the sort, the entl is false"<<std::endl;
								return z3::sat;
							}		
						}else{
							std::vector<std::vector<z3::expr_vector>>::iterator begin = sort_set_tmp.begin();
							std::vector<z3::expr>::iterator begin_pf = sort_set_tmp_pf.begin();
							sort_set_tmp.insert(begin,sort_tmp_new2);
							sort_set_tmp_pf.insert(begin_pf,sort_tmp_pf_new2);
						}
					}
				}
			}
		}	
	}
	
	return z3::unsat;
//	for(int i = 0; i < sorts.size(); i++){
//		for(int j = 0; j < sorts[i].size(); j++){
//			std::cout<<sorts[i][j];
//		}
//		std::cout<<std::endl;
//	}
}

bool PASolver::check_sort(std::vector<z3::expr_vector> &sort,z3::expr_vector &headsA, z3::expr_vector &tailsA, z3::expr_vector &tailsB){
	//check last eq_class
	int flag=0;//0:empty;1:A;2:B;3:AB
	z3::expr_vector last_add = sort.back();
	for(int j = 0; j<last_add.size();j++){
		if(find_index(tailsA,last_add[j])!=-1){
			if(flag == 2) flag=3;
			if(flag == 0) flag=1;
		}
		if(find_index(tailsB,last_add[j])!=-1){
			if(flag == 1) flag=3;
			if(flag == 0) flag=2;
		}
		if(flag == 3) break;	
	}
	if(flag != 3) return false;

	//check first address
	flag=0;//0:empty;1:A
	z3::expr_vector first_add = sort[0];
	for(int j = 0; j<first_add.size();j++){
		if(find_index(headsA,first_add[j])!=-1){
			flag=1;
			break;
		}
	}
	if(flag != 1) return false;

	//check address cannot match
	bool match = true; bool isHead=true;
	for(int j=1;j<sort.size()-1;j++){//each eq_class
		if(sort[j].size()==3){//tail(ax),head(ay),tail(b)
			isHead=true;
		}else if(sort[j].size()==2){
			if((find_index(headsA,sort[j][0])!=-1&&find_index(tailsA,sort[j][0])!=-1)//tail(ax),head(ay),tail(b)
			 ||(find_index(headsA,sort[j][1])!=-1&&find_index(tailsA,sort[j][1])!=-1)
			 ||(find_index(headsA,sort[j][0])!=-1&&find_index(tailsB,sort[j][0])!=-1)
			 ||(find_index(headsA,sort[j][1])!=-1&&find_index(tailsB,sort[j][1])!=-1)
			 ||(find_index(tailsA,sort[j][0])!=-1&&find_index(tailsB,sort[j][0])!=-1)
			 ||(find_index(tailsA,sort[j][1])!=-1&&find_index(tailsB,sort[j][1])!=-1)){
			 	isHead=true;
			 }else if((find_index(headsA,sort[j][0])!=-1&&find_index(tailsB,sort[j][1])!=-1)//head(a),tail(b)
			 ||(find_index(headsA,sort[j][1])!=-1&&find_index(tailsB,sort[j][0])!=-1)){
				return false;
			}else if(find_index(tailsB,sort[j][0])!=-1||find_index(tailsB,sort[j][1])!=-1){//tail(a),tail(b)
				isHead=false;
			}else{//tail(ax),head(ay)
				isHead=true;
			}
			
		}else{
			if(find_index(headsA,sort[j][0])!=-1&&find_index(tailsA,sort[j][0])!=-1&&find_index(tailsB,sort[j][0])!=-1){//tail(ax),head(ay),tail(b)
				isHead=true;
			}else if(find_index(headsA,sort[j][0])!=-1&&find_index(tailsB,sort[j][0])!=-1){
				return false;
			}else if(find_index(tailsA,sort[j][0])!=-1&&find_index(tailsB,sort[j][0])!=-1){
				isHead=true;
			}else if(find_index(tailsB,sort[j][0])!=-1){
				if(!isHead) return false;
			}else if(find_index(headsA,sort[j][0])!=-1){
				isHead=true;
			}else{
				isHead=false;
			}
		}
		
	} 
	return true;
}

void PASolver::sort_space(std::vector<z3::expr_vector> sort, z3::expr &spaceA, z3::expr &spaceB, z3::expr_vector &tailsA,z3::expr_vector &tailsB){
	z3::expr_vector argsA = get_space_args(spaceA);
	z3::expr_vector argsB = get_space_args(spaceB);
	z3::expr_vector argsA_sorted(z3_ctx);
	z3::expr_vector argsB_sorted(z3_ctx);
	for(int i = 0; i<sort.size();i++){
		for(int j = 0; j<sort[i].size();j++){
			if(find_index(tailsA,sort[i][j]) != -1){
				int index = find_index(tailsA,sort[i][j]);
				argsA_sorted.push_back(argsA[index]);
			}
			if(find_index(tailsB,sort[i][j]) != -1){
				int index = find_index(tailsB,sort[i][j]);
				argsB_sorted.push_back(argsB[index]);
			}
		}
	}
	
//for(int i = 0;i<sort.size();i++) {
//	std::cout<<sort[i];
//}
//std::cout<<std::endl;
//std::cout<<argsA_sorted<<std::endl;
//std::cout<<argsB_sorted<<std::endl;
//system("read REPLY");
	spaceA = new_sep(argsA_sorted);
	spaceB = new_sep(argsB_sorted);
}

z3::expr_vector PASolver::merge(z3::expr_vector set1,z3::expr_vector set2){
	z3::expr_vector res=set1;
	for(int i=0;i<set2.size();i++){
		res.push_back(set2[i]);
	}
	return res;
}

void PASolver::remove_emp_P(std::vector<z3::expr_vector>& sort,z3::expr& spaceC){
	z3::expr_vector args(z3_ctx);
	for(int i=0;i<spaceC.num_args();i++){
		if(isP(spaceC.arg(i))){
			z3::expr atom = spaceC.arg(i);
			z3::expr h = head(atom);
			z3::expr t = tail(atom);
			z3::expr_vector eq_class = get_eq_class(sort,h);
			if(find_index(eq_class, t)==-1) args.push_back(spaceC.arg(i));
		}else{
			args.push_back(spaceC.arg(i));
		}
	}
	spaceC = new_sep(args);
}

bool PASolver::check_sorted_entl(std::vector<z3::expr_vector> sort, z3::expr dataC, z3::expr spaceC, z3::expr dataD, z3::expr spaceD, z3::expr_vector &ex_var_set, z3::expr_vector &pi_ex_set){
	remove_emp_P(sort,spaceC);
	
	z3::expr_vector tmp_ex_var_set=ex_var_set;
	z3::expr_vector tmp_pi_ex_set=pi_ex_set;
	z3::expr_vector tailsC = get_space_tails(spaceC);
	z3::expr_vector headsC = get_space_heads(spaceC);
	z3::expr atom_d1=spaceD.arg(0);
	z3::expr tail_d1 = tail(atom_d1);
	
	//find if tail_ci=tail_d1
	int c_index = find_index(tailsC,tail_d1);
	if(c_index == -1){
		z3::expr_vector eq_class = get_eq_class(sort,tail_d1);
		for(int i = 0;i<eq_class.size();i++){
			c_index = find_index(tailsC,eq_class[i]);
			if(c_index!=-1) break;
		}
	}
	
	z3::expr_vector argsC1(z3_ctx);
	z3::expr_vector argsC2(z3_ctx);
	z3::expr_vector argsD1(z3_ctx);
	for(int i = 1;i<spaceD.num_args();i++) argsD1.push_back(spaceD.arg(i));
	
	if(c_index != -1){//tail_d1=tail_c_index
		for(int i = 0;i<=c_index;i++) argsC1.push_back(spaceC.arg(i));
		for(int i = c_index+1;i<spaceC.num_args();i++) argsC2.push_back(spaceC.arg(i));
		
		if(!match_atom(dataC,new_sep(argsC1),dataD,atom_d1,ex_var_set,pi_ex_set)) return false;
		if(spaceD.num_args()==1){
			return true;	
		}else{
			return check_sorted_entl(sort,dataC,new_sep(argsC2),dataD,new_sep(argsD1),ex_var_set,pi_ex_set);
		}
	}else{//head(c_{c_index}) < tail(d_1) < tail(c_{c_index})
		int tmp=-1;
		for(int i = 0;i<sort.size();i++){
			if(find_index(sort[i],tail_d1)!=-1) tmp=find_index(sort[i],tail_d1);
			if(tmp != -1){
				for(int j = 0;j<sort[i+1].size();j++){//i+1 is available because tail(dn) == tail(c_m)
					if(find_index(tailsC,sort[i+1][j])!=-1){
						c_index = find_index(tailsC,sort[i+1][j]);
						break;
					}
				}
				if(c_index!=-1) break;
			}
		}
		
		z3::expr atom_ck = spaceC.arg(c_index);
		if(spaceC.arg(c_index).decl().name().str() == "blk"){//c_k is hls or blk atom
			for(int i = 0;i<c_index;i++) argsC1.push_back(spaceC.arg(i));
			z3::expr atom1 = newblk(head(atom_ck),tail_d1);
			z3::expr atom2 = newblk(tail_d1,tail(atom_ck));
			argsC1.push_back(atom1);
			argsC2.push_back(atom2);
			for(int i = c_index+1;i<spaceC.num_args();i++) argsC2.push_back(spaceC.arg(i));
			
			if(!match_atom(dataC,new_sep(argsC1),dataD,atom_d1,ex_var_set,pi_ex_set)) return false;
			return check_sorted_entl(sort,dataC,new_sep(argsC2),dataD,new_sep(argsD1),ex_var_set,pi_ex_set);	
		}else{//ck is P
			z3::expr_vector emp_pi_set(z3_ctx);
			pi_ex_set=emp_pi_set;
			isP(atom_ck);
			Predicate_SLAH *ck_pdef = dynamic_cast<Predicate_SLAH *>(m_problem->getPredicate()); 
			int zsize=ck_pdef->get_z_size();
			int vsize=ck_pdef->get_v_size();
			z3::func_decl p_fun = ck_pdef->get_fun();
			HeapChunk* hckdef = m_problem->getHeapChunk();
			int hck_exsize = hckdef->get_ex_size();
			
			z3::expr_vector z_newvar_set(z3_ctx);
			for(int k=0;k<zsize;k++){
				std::string z_newvar_name = "[z,"+std::to_string(z_newvar_index)+"]";z_newvar_index++;
				z3::expr z_newvar = z3_ctx.constant(z3_ctx.str_symbol(z_newvar_name.c_str()),z3_ctx.int_sort());
				z_newvar_set.push_back(z_newvar);
			}
			z3::expr_vector div1_args(z3_ctx);
			z3::expr_vector div2_args(z3_ctx);
			
			for(int i=0;i<zsize+1;i++){//x,z
				div1_args.push_back(atom_ck.arg(i));
			}
			div1_args.push_back(tail(atom_d1));//tail(d1)
			div2_args.push_back(tail(atom_d1));//tail(d1)
			for(int i=0;i<zsize;i++){//u'
				div1_args.push_back(z_newvar_set[i]);
				div2_args.push_back(z_newvar_set[i]);
			}
			for(int i=0;i<vsize;i++){//v
				div1_args.push_back(atom_ck.arg(2*(1+zsize)+i)); 
			}
			z3::expr div1=p_fun(div1_args);
			for(int i=zsize+1;i<atom_ck.num_args();i++){
				div2_args.push_back(atom_ck.arg(i));
			}
			z3::expr div2=p_fun(div2_args);
			
			z3::expr_vector atomset(z3_ctx);
			for(int i = 0;i<c_index;i++) atomset.push_back(spaceC.arg(i));
			atomset.push_back(div1);
			atomset.push_back(div2);
			for(int i = c_index+1;i<spaceC.num_args();i++) atomset.push_back(spaceC.arg(i));
			
			z3::expr unfoldspaceC=new_sep(atomset);
			z3::expr con=get_abstraction(dataC,unfoldspaceC);
			z3_sol.reset();
			z3_sol.add(con);
			if(z3_sol.check()==z3::sat){
				z3::expr_vector ex_var_set1=tmp_ex_var_set;
				z3::expr_vector pi_ex_set1=tmp_pi_ex_set;
				z3::expr_vector atomset1(z3_ctx);
				for(int i = 0;i<c_index;i++) atomset1.push_back(spaceC.arg(i));
				atomset1.push_back(div1);
				if(!match_atom(con,new_sep(atomset1),dataD,atom_d1,ex_var_set1,pi_ex_set1)) return false;
				z3::expr_vector atomset2(z3_ctx);
				atomset2.push_back(div2);
				for(int i = c_index+1;i<spaceC.num_args();i++) atomset2.push_back(spaceC.arg(i));
				if(!check_sorted_entl(sort,con,new_sep(atomset2),dataD,new_sep(argsD1),ex_var_set1,pi_ex_set1)) return false;
				pi_ex_set=merge(pi_ex_set,pi_ex_set1);
				
				for(int i=0;i<hckdef->get_space().size();i++){
					z3::expr_vector ck_div1_args(z3_ctx);
					z3::expr_vector ck_div2_args(z3_ctx);
					z3::expr_vector ck_div3_args(z3_ctx);
					
					for(int j=0;j<zsize+1;j++){//x,z
						ck_div1_args.push_back(atom_ck.arg(j));
					}
					
					std::string x1_newvar_name = "[x1,"+std::to_string(z_newvar_index)+"]";
					z3::expr x1_newvar = z3_ctx.constant(z3_ctx.str_symbol(x1_newvar_name.c_str()),z3_ctx.int_sort());
					std::string x2_newvar_name = "[x2,"+std::to_string(z_newvar_index)+"]";
					z3::expr x2_newvar = z3_ctx.constant(z3_ctx.str_symbol(x2_newvar_name.c_str()),z3_ctx.int_sort());
					ck_div1_args.push_back(x1_newvar);//x1
					ck_div2_args.push_back(x1_newvar);//x1
					
					z3::expr_vector z_newvar_set1(z3_ctx);
					z3::expr_vector z_newvar_set2(z3_ctx);
					for(int k=0;k<zsize;k++){//u'
						std::string z_newvar_name1 = "[z,"+std::to_string(z_newvar_index)+"]";z_newvar_index++;
						z3::expr z_newvar1 = z3_ctx.constant(z3_ctx.str_symbol(z_newvar_name1.c_str()),z3_ctx.int_sort());
						z_newvar_set1.push_back(z_newvar1);
						ck_div1_args.push_back(z_newvar1);//u'
						ck_div2_args.push_back(z_newvar1);//u'
						std::string z_newvar_name2 = "[z,"+std::to_string(z_newvar_index)+"]";z_newvar_index++;
						z3::expr z_newvar2 = z3_ctx.constant(z3_ctx.str_symbol(z_newvar_name2.c_str()),z3_ctx.int_sort());
						z_newvar_set2.push_back(z_newvar2);
					}
					
					z3::expr_vector new_ex_var(z3_ctx);
					for(int k=0;k<hck_exsize;k++){//w
						std::string w_newvar_name = "[w,"+std::to_string(w_newvar_index)+"]";w_newvar_index++;
						z3::expr w_newvar = z3_ctx.constant(z3_ctx.str_symbol(w_newvar_name.c_str()),z3_ctx.int_sort());
						new_ex_var.push_back(w_newvar);
					}
					
					ck_div2_args.push_back(x2_newvar);//x2
					ck_div3_args.push_back(x2_newvar);//x2
					for(int j=0;j<zsize;j++){
						ck_div2_args.push_back(z_newvar_set2[j]);//u''
						ck_div3_args.push_back(z_newvar_set2[j]);//u''
					}
					
					for(int j=0;j<zsize+1;j++){
						ck_div3_args.push_back(atom_ck.arg(zsize+1+j));//y,u
					}
					
					for(int j=0;j<vsize;j++){//v
						ck_div1_args.push_back(atom_ck.arg(2*(zsize+1)+j));
						ck_div2_args.push_back(atom_ck.arg(2*(zsize+1)+j));
						ck_div3_args.push_back(atom_ck.arg(2*(zsize+1)+j));
					}
					
					z3::expr ck_div1=p_fun(ck_div1_args);
					z3::expr ck_div3=p_fun(ck_div3_args);
					
					z3::expr pi=hckdef->get_data();
					par2arg(pi,hckdef->get_pars(),ck_div2_args,new_ex_var);
					z3::expr_vector sigma(z3_ctx);
					for(int k=0;k<hckdef->get_space().size();k++){
						z3::expr bk = hckdef->get_space()[k];
						par2arg(bk,hckdef->get_pars(),ck_div2_args,new_ex_var);
						sigma.push_back(bk);
					}
					
					z3::expr_vector atomset(z3_ctx);
					for(int j = 0;j<c_index;j++) atomset.push_back(spaceC.arg(j));
					atomset.push_back(ck_div1);
					for(int j=0;j<sigma.size();j++) atomset.push_back(sigma[j]);
					atomset.push_back(ck_div3);
					for(int j = c_index+1;j<spaceC.num_args();j++) atomset.push_back(spaceC.arg(j));
					
					z3::expr data=dataC && pi;
					z3::expr unfoldspaceC=new_sep(atomset);
					z3::expr con=get_abstraction(data,unfoldspaceC);
					z3_sol.reset();
					z3_sol.add(con);
					if(z3_sol.check()==z3::sat){
						z3_sol.reset();
						z3::expr con1 = con&&tail(atom_d1)==tail(sigma[i]);
						z3_sol.add(con1);
						if(z3_sol.check()==z3::sat){
							z3::expr_vector ex_var_set2=tmp_ex_var_set;
							z3::expr_vector pi_ex_set2=tmp_pi_ex_set;
							z3::expr_vector atomset1(z3_ctx);
							for(int j = 0;j<c_index;j++) atomset1.push_back(spaceC.arg(j));
							atomset1.push_back(ck_div1);
							for(int j=0;j<=i;j++) atomset1.push_back(sigma[j]);
							if(!match_atom(con1,new_sep(atomset1),dataD,atom_d1,ex_var_set2,pi_ex_set2)) return false;
							z3::expr_vector atomset2(z3_ctx);
							for(int j=i+1;j<sigma.size();j++) atomset2.push_back(sigma[j]);
							atomset2.push_back(ck_div3);
							for(int j = c_index+1;j<spaceC.num_args();j++) atomset2.push_back(spaceC.arg(j));
							if(check_entl_get_relations(con1,new_sep(atomset2),dataD,new_sep(argsD1),ex_var_set2,pi_ex_set2)==z3::sat) return false;
							pi_ex_set=merge(pi_ex_set,pi_ex_set2);
						}
						
						
						if(sigma[i].decl().name().str()=="blk"){
							z3_sol.reset();
							z3::expr con2 = con&&head(sigma[i])<tail(atom_d1)&&tail(atom_d1)<tail(sigma[i]);
							z3_sol.add(con2);
							if(z3_sol.check()==z3::sat){
								z3::expr_vector ex_var_set3=tmp_ex_var_set;
								z3::expr_vector pi_ex_set3=tmp_pi_ex_set;
								z3::expr_vector atomset1(z3_ctx);
								for(int j = 0;j<c_index;j++) atomset1.push_back(spaceC.arg(j));
								atomset1.push_back(ck_div1);
								for(int j=0;j<i;j++) atomset1.push_back(sigma[j]);
								atomset1.push_back(sigma[i].decl()(sigma[i].arg(0),tail(atom_d1)));
								if(!match_atom(con2,new_sep(atomset1),dataD,atom_d1,ex_var_set3,pi_ex_set3)) return false;
								z3::expr_vector atomset2(z3_ctx);
								atomset2.push_back(sigma[i].decl()(tail(atom_d1),sigma[i].arg(1)));
								for(int j=i+1;j<sigma.size();j++) atomset2.push_back(sigma[j]);
								atomset2.push_back(ck_div3);
								for(int j = c_index+1;j<spaceC.num_args();j++) atomset2.push_back(spaceC.arg(j));
								if(check_entl_get_relations(con2,new_sep(atomset2),dataD,new_sep(argsD1),ex_var_set3,pi_ex_set3)==z3::sat) return false;
								pi_ex_set=merge(pi_ex_set,pi_ex_set3);
							}
						}
					}
					
				}
			}
		}
	}
	
	return true;
}
