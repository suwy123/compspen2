#include "solver_slid_int/listsolver.h"

extern z3::context z3_ctx;

/**
 *###################### listsolver ####################################
 */
/**
 * check whether all predicate definitions are corresponding to userdef constraints
 */
//void listsolver::check_preds() {
//        for (int i=0; i<m_ctx.pred_size(); i++) {
//                predicate pred = m_ctx.get_pred(i);
//                if (pred.is_list()) {
//                        listchecker lc(pred);
//                        lc.check_args();
//                        lc.check_rec_rules();
//                }
//        }
//}

listsolver::listsolver(Problem* problem) : 
	m_problem(problem),delta_ge1_predicate(z3_ctx),
	m_phi_abs(z3_ctx),m_phi_space(z3_ctx),
	m_psi_abs(z3_ctx),m_psi_space(z3_ctx),
	m_omega_eq_size(0) {}

void listsolver::solve() {
    string result = "";
    if (m_problem->isSat()) {
        if(check_sat() == z3::sat){
        	result = "sat";
		}else{
			result = "unsat";
		}
    } else {
        if(check_entl() == z3::sat){
        	result = "sat";
		}else{
			result = "unsat";
		}
    }
    cout << "result: " << result <<endl;
}

/**
 * check sat, negf in m_ctx
 * or check entl, negf |= posf
 * @return z3::check_result
 */
z3::check_result listsolver::check_sat() {
        //logger() << "list sat problem: " << std::endl;
        // 1.1 compute all phi_pd
        compute_all_data_closure();
        z3::expr formula = m_problem->getPhi();
        z3::expr_vector f_new_bools(z3_ctx);
        z3::expr space(z3_ctx);
        z3::expr data(z3_ctx);
        z3::expr f_abs = get_abstraction(formula, data, space, f_new_bools);
        z3::solver s(z3_ctx);
        s.add(f_abs);
        z3::check_result result = s.check();
        return result;
}



//z3::model listsolver::get_model() {
//    z3::model m = s.get_model();
//
//    z3::expr formula = m_ctx.get_negf();
//    z3::expr data(z3_ctx());
//    z3::expr space(z3_ctx());
//    get_data_space(formula, data, space);
//
//    std::ofstream out("model.dot");
//    if (!out) return m;
//    out<<"digraph g {\n";
//    out<<"graph [rankdir=\"LR\"];\n";
//    out<<"node [fontsize=\"16\" shape=\"record\"];\n";
//
//    out << "subgraph cluster1 {\n label=\"(Stack)\"\n";
//
//    int num = m.num_consts();
//
//    out << "satck [label=\"";
//    for (int i=0; i<num; i++) {
//        z3::func_decl x = m.get_const_decl(i);
//        z3::expr x_interp = m.get_const_interp(x);
//        if (x.name().str().find("[") != 0) {
//            out <<"|" <<x.name() << ":" << x_interp;
//        }
//    }
//    out << "\"]\n";
//
//
//
//    out << "}\n";
//
//    out << "subgraph cluster2 {\n label=\"(Heap)\"\n";
//    int n = space.num_args();
//
//    for (int i=0; i<n; i++) {
//        //1.1 fetch atom
//        z3::expr atom = space.arg(i);
//        //1.2 get_model_str
//        std::string atom_str = get_model_of_atom(m, atom, i, n);
//        out << atom_str;
//    }
//
//    out<<"}\n}\n";
//
//    out.close();
//    return s.get_model();
//}


/**
 * get interpretation of exp in m
 * @param m : model
 * @param expr : exp
 * @return expr
 */
//z3::expr listsolver::get_interp(z3::model &m, z3::expr exp) {
//
//        z3::expr nil = z3_ctx().int_const("nil");
//        // std::cout << "exp: " << exp << std::endl;
//        if (exp.get_sort().sort_kind() == Z3_UNINTERPRETED_SORT) {
//                z3::expr exp_int = z3_ctx().int_const(exp.to_string().c_str());
//                if (m.has_interp(exp_int.decl())) {
//                        return m.get_const_interp(exp_int.decl());
//                } else if (exp.to_string().find("var_")==0) {
//                        return exp;
//                }
//        } else {
//                if (m.has_interp(exp.decl())) {
//                        return m.get_const_interp(exp.decl());
//                }
//        }
//        return nil;
//}

/**
 * get abstraction of formula
 * @param formula :
 * @param space : the space part of formula
 * @param new_bools : aux output new bool vars
 * @return
 */
z3::expr listsolver::get_abstraction(z3::expr &formula, z3::expr& data, z3::expr& space, z3::expr_vector& new_bools) {
        // logger() << "get abstraction of formula: " << formula << std::endl;
        // 1.2 formula -> (delta \and sigma)
        // z3::expr data(z3_ctx());
        get_data_space(formula, data, space);

        z3::expr f_abs = data;
        // 1.3 space part
        if(Z3_ast(space) != nullptr){
        	f_abs = f_abs && abs_space(space, new_bools);
        	// 1.4 sep (\phi_star)
        	f_abs = f_abs && abs_phi_star(new_bools);
		} 
        
        return f_abs;
}

z3::expr listsolver::get_abstraction(z3::expr &formula, z3::expr_vector& new_bools) {

        z3::expr space(z3_ctx);
        z3::expr data(z3_ctx);
        return get_abstraction(formula, data, space, new_bools);
}

void listsolver::get_data_space(z3::expr &formula, z3::expr &data, z3::expr &space) {
	if(Z3_ast(formula)==nullptr) return;
	expr_vector data_items(z3_ctx);
    data_items.push_back(z3_ctx.int_const("nil") == 0); // nil == 0
    int num = formula.num_args();
    int data_num = num-1;
    
    Predicate_SLID_INT *pred = dynamic_cast<Predicate_SLID_INT *>(m_problem->getPredicate()); 
    if(num>0 && expr_tool::is_fun(formula.arg(num-1),"sep")){
    	space = formula.arg(num-1);
	}else if(num>0 && (expr_tool::is_fun(formula.arg(num-1),"pto")||expr_tool::is_fun(formula.arg(num-1),pred->get_fun().name().str()))){
		z3::sort range = z3_ctx.bool_sort();
	    z3::sort_vector domains(z3_ctx);
	    domains.push_back(range);
	    z3::func_decl sep_f = z3_ctx.function("sep", domains, range);
	    space = sep_f(formula.arg(num-1));
	}else{
		//z3::sort_vector domains(z3_ctx);
	    //z3::func_decl sep_f = z3_ctx.function("sep", domains, range);
	    //z3::func_decl sep_f = z3_ctx.function("sep", 0, 0, z3_ctx.bool_sort());
	    //space = sep_f();
		data_num = num;
	}
	
	for (int i=0; i<data_num; i++) {
		if (formula.arg(i).num_args()==2 && expr_tool::is_location(formula.arg(i).arg(0))) {
	        z3::expr item = formula.arg(i);
	        z3::expr z1_int = z3_ctx.int_const(item.arg(0).to_string().c_str());
	        z3::expr z2_int = z3_ctx.int_const(item.arg(1).to_string().c_str());
	        if (item.decl().name().str() == "distinct") {
	            data_items.push_back(z1_int != z2_int);
	        } else {
	            data_items.push_back(z1_int == z2_int);
	        }
	    }else {
	        data_items.push_back(formula.arg(i));
	    }
    } 
    
//    for (int i=0; i<data_num; i++) {
//    	expr item1 = z3_ctx.int_const(formula.arg(i).arg(0).to_string().c_str());
//        expr item2 = z3_ctx.int_const(formula.arg(i).arg(1).to_string().c_str());
//        string fname = formula.arg(i).decl().name().str();
//        if (fname == "distinct") {
//            data_items.push_back(item1 != item2);
//        } else {
//            data_items.push_back(item1 == item2);
//        }
//    }
    data = mk_and(data_items);
    
}

z3::expr listsolver::abs_space(z3::expr &space, z3::expr_vector& new_bools) {
    // 1.space atoms -> abs (\phi_sigma)
    z3::expr f_abs(z3_ctx);
    z3::expr b_false = z3_ctx.bool_val(false);
    for (int i=0; i<space.num_args(); i++) {
        //1.1 fetch atom
        z3::expr atom = space.arg(i);

        z3::expr atom_f = pred2abs(atom, i, new_bools);
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

z3::expr listsolver::abs_phi_star(z3::expr_vector& new_bools) {
    z3::expr phi_star = z3_ctx.bool_val(true);
    // phi_star
    for (int i=0; i<new_bools.size(); i++) {
        for (int j=i+1; j<new_bools.size(); j++) {
            std::string b1_name = new_bools[i].to_string();
            std::string b2_name = new_bools[j].to_string();
            int i1 = b1_name.find(",");
            int i2 = b2_name.find(",");
            std::string z1_name = b1_name.substr(2, i1-2);
            std::string z1_i = b1_name.substr(i1+1, b1_name.length()-i1-3);
            std::string z2_name = b2_name.substr(2, i2-2);
            std::string z2_i = b2_name.substr(i2+1, b2_name.length()-i2-3);

            if (z1_i != z2_i) {
                // add imply atom
                // z3::expr imply = implies( (z3_ctx().int_const(z1_name.c_str()) == z3_ctx().int_const(z2_name.c_str()) && new_bools[i]), !new_bools[j]);
                z3::expr imply = ! ((z3_ctx.int_const(z1_name.c_str()) == z3_ctx.int_const(z2_name.c_str()) && new_bools[i]) && new_bools[j]);
                // if (Z3_ast(phi_star) == 0) {
                //    phi_star = imply;
                // } else {
                phi_star = phi_star && imply;
                //}
            }
        }
    }
    return phi_star;
}

/**
 * check sat, negf in m_ctx
 * or check entl, negf |= posf
 * @return z3::check_result
 */
z3::check_result listsolver::check_entl() {
        std::cout << "list entl problem:\n";

        z3::solver s(z3_ctx);
        z3::expr f_abs = z3_ctx.bool_val(true);
        s.add(f_abs);
        z3::check_result result = s.check();

        // TO checking
//        assert(m_ctx.pred_size() == 1);

        // 1.1 compute all phi_pd
        compute_all_data_closure();

        // \varphi
        z3::expr phi = m_problem->getPhi();
        // \psi
        z3::expr psi = m_problem->getPsi();

        // logger() << "phi: " << phi << std::endl;
        // logger() << "psi: " << psi << std::endl;




        ////
        //  check phi
        z3::expr_vector phi_new_bools(z3_ctx);
        z3::expr phi_space(z3_ctx);
        z3::expr phi_data(z3_ctx);
        z3::expr phi_abs = get_abstraction(phi, phi_data, phi_space, phi_new_bools);

        m_phi_abs = phi_abs;
        m_phi_space = phi_space;

        s.reset();
        s.add(phi_abs);
        z3::check_result phi_res = s.check();
        std::cout << "phi sat res: " << phi_res << std::endl;
        if (phi_res == z3::unsat) return z3::unsat;

        // check psi
        z3::expr_vector psi_new_bools(z3_ctx);
        z3::expr psi_space(z3_ctx);
        z3::expr psi_data(z3_ctx);
std::cout<<"psi: "<<psi<<std::endl;
        z3::expr psi_abs = get_abstraction(psi, psi_data, psi_space, psi_new_bools);
        m_psi_abs = psi_abs;
        m_psi_space = psi_space;
        s.reset();
        s.add(psi_abs);
        z3::check_result psi_res = s.check();
        std::cout << "psi sat res: " << psi_res << std::endl;
        if (psi_res==z3::unsat) return z3::sat;

        // check special: either emp heap
        if (is_emp(phi_abs, phi_space)) {
            return z3::unsat;
        }

        if (is_emp(psi_abs, psi_space)) {
            // ....
            return z3::sat;
        }

        // check var subset

        // const_set, lconst_set
        std::set<z3::expr, exprcomp> phi_const_set;
        expr_tool::get_consts(phi, phi_const_set);
        z3::expr nil_const = z3_ctx.int_const("nil");
        m_phi_const_vec.push_back(nil_const);
        for (auto con : phi_const_set) {
            if (expr_tool::is_location(con)) {
                m_phi_const_vec.push_back(con);
            }
        }

        std::set<z3::expr, exprcomp> psi_const_set;
        expr_tool::get_consts(psi, psi_const_set);
        m_psi_const_vec.push_back(nil_const);
        for (auto con : psi_const_set) {
            if (expr_tool::is_location(con)) {
                m_psi_const_vec.push_back(con);
            }
        }
        phi_const_set.insert(nil_const);
        psi_const_set.insert(nil_const);

        bool is_subset = expr_tool::is_sub_set(psi_const_set, phi_const_set);
        // logger() << "psi_const_set size: " << psi_const_set.size() << std::endl;
        // logger() << "phi_const_set size: " << phi_const_set.size() << std::endl;

        std::cout << "psi_const_set is subset of phi_const_set: " <<  is_subset << std::endl;
        if(!is_subset) return z3::sat; // entl is false



        //  check Abs(\varphi) |= \exists Z. Abs(\psi)

        z3::expr check_f = phi_abs && !psi_data;
std::cout<<"check_f: "<<check_f<<std::endl;
        s.reset();
        s.add(check_f);
        if (s.check() == z3::sat) {
        	z3::model m = s.get_model();
			int num = m.num_consts();			
//			for (int i=0; i<num; i++) {
//			    z3::func_decl x = m.get_const_decl(i);
//			    z3::expr x_interp = m.get_const_interp(x);
//			    if (x.name().str().find("[") != 0) {
//			        std::cout <<"|" <<x.name() << ":" << x_interp<<std::endl;
//			    }
//			}
            std::cout << "phi abs not |= psi data\n";
            return z3::sat;
        }
        /*
        z3::expr_vector Z = psi_new_bools;
        for (int i=0; i<m_ctx.psi_space.num_args(); i++) {
                std::string k_i_name = logger().string_format("[k,%d]", i);
                Z.push_back(z3_ctx().int_const(k_i_name.c_str()));
        }


        // logger() << "Z: " << Z << std::endl;
        // z3::expr phi_abs_entl_psi_abs =  phi_abs && (z3::forall(Z, !psi_abs)); //
        // z3::expr phi_abs_entl_psi_abs = z3::implies(phi_abs, (z3::forall(Z, psi_abs))); //
        // logger() << "phi_abs_entl_psi_abs: " << phi_abs_entl_psi_abs << std::endl;

        // z3::solver s1(z3_ctx(), "LIA");
        // s1.add(phi_abs_entl_psi_abs);
        // z3::check_result entl_abs_res = z3::unsat;//s1.check();
        // std::cout << "entl abs result: " << entl_abs_res << std::endl;
        // logger() << "\nphi_abs entl forall psi_abs result: " << entl_abs_res << std::endl;
        // if (entl_abs_res == z3::sat) {
         // std::cout <<s1.get_model();
         // std::cout << "phi_abs entl forall psi_abs is false!\n";
        // return z3::sat;
        //}
        // logger() << "model: " << ss.get_model() << std::endl;
        // if (pep_res == z3::sat || pep_res == z3::unknown) return z3::unsat; // if sat return unsat
        */


        // construct graph
        // phi phi_const_vec
        //std::vector<z3::expr> phi_const_vec;
        // std::vector<int> phi_const_eq_class_vec(phi_const_set.size(), -1);
        // init_int_vector(m_ctx.phi_const_eq_class_vec, phi_const_set.size());


        // int phi_eq_class_size = get_const_vec_and_eq_class(phi, phi_abs, m_ctx.phi_const_vec, m_ctx.phi_const_eq_class_vec);
        // m_ctx.omega_eq_size = phi_eq_class_size;
        // std::vector<z3::expr> psi_const_vec;
        // std::vector<int> psi_const_eq_class_vec(psi_const_set.size(), -1);
        // init_int_vector(m_ctx.psi_const_eq_class_vec, psi_const_set.size() + 1);
        // int psi_eq_class_size = get_const_vec_and_eq_class(psi, psi_abs, m_ctx.psi_const_vec, m_ctx.psi_const_eq_class_vec);
        // std::vector<int> psi_eq_to_eq_table(psi_eq_class_size, -1);
        // init map table
        // init_int_vector(m_ctx.psi_eq_to_eq_table, psi_eq_class_size);

        listgraph g_phi;
        listgraph g_psi;
        std::vector<std::set<int> > phi_eq_class_vec;
        get_eq_class(phi_abs, m_phi_const_vec, phi_eq_class_vec, m_phi_const_eq_class_vec);

        // logger() << phi_abs << std::endl;

        for (int i=0; i<m_phi_const_eq_class_vec.size(); i++) {
            std::cout << m_phi_const_vec[i] << ": " << m_phi_const_eq_class_vec[i] << std::endl;
        }



        construct_graph(m_phi_const_vec, m_phi_const_eq_class_vec, phi_eq_class_vec, m_phi_space, g_phi);
        g_phi.print(m_phi_const_vec, m_phi_space, "g_phi.dot");
        m_omega_eq_size = phi_eq_class_vec.size();
        /*
        std::vector<int> cc_cycle_num = g_phi.get_cc_cycle_num();
        for (int i=0; i<cc_cycle_num.size(); i++) {
            std::cout << "cc " << i << " number: "  << cc_cycle_num[i] << std::endl;
            for (int j=0; j<cc_cycle_num[i]; j++) {
                std::pair<int, int> coord = std::make_pair(i, j);
                std::vector<graph::edge_t> edge_cycle = g_phi.get_edge_cycle(coord);
                std::cout << "cycle " << j << " size: " << edge_cycle.size() << std::endl;

            }

        }
        */

        std::vector<std::set<int> > psi_eq_class_vec;
        get_eq_class(psi_abs, m_psi_const_vec, psi_eq_class_vec, m_psi_const_eq_class_vec);
        construct_graph(m_psi_const_vec, m_psi_const_eq_class_vec, psi_eq_class_vec, m_psi_space, g_psi);
        g_psi.print(m_psi_const_vec, m_psi_space, "g_psi.dot");


        // 1.5 allocating plans

        if (check_allocating_plans(g_phi, g_psi, phi_abs)) {
            return z3::unsat;
        } else {
            return z3::sat;
        }
}

bool listsolver::is_emp(z3::expr& abs, z3::expr& space) {
    if (Z3_ast(space) == nullptr) {
        return true;
    }
    z3::expr_vector allocated_vec(z3_ctx);
    for (int i=0; i<space.num_args(); i++) {
        if (expr_tool::is_fun(space.arg(i), "pto")) return false;

        std::string source_name = space.arg(i).arg(0).to_string();
        if (source_name == "nil") continue;
        std::string bool_name = "["+source_name+","+to_string(i)+"]";
        allocated_vec.push_back(z3_ctx.bool_const(source_name.c_str()));
    }

    if (allocated_vec.size() > 0) {
        z3::expr check_f = abs && z3::mk_or(allocated_vec);
        z3::solver s(z3_ctx);
        s.add(check_f);
        if (s.check() == z3::unsat) {
            return true;
        } else {
            return false;
        }
    }
    return true;
}

/** \brief check all feasible allocating plans
 *
 * \param g_phi graph phi
 * \param g_psi graph psi
 * \param phi_abs init phi_abs
 * \return true or false
 *
 */

bool listsolver::check_allocating_plans(listgraph& g_phi, listgraph& g_psi, z3::expr& phi_abs) {
        std::vector<int> cc_cycle_num = g_phi.get_cc_cycle_num();
        int cc_num = cc_cycle_num.size();
        std::cout << "cc_num: " << cc_num << std::endl;
        int cc_cycle_total_num = 0;
        for (int i=0; i<cc_num; i++) {
            cc_cycle_total_num += cc_cycle_num[i];
            std::cout << "cc_cycle_num: " << i << ": " << cc_cycle_num[i] <<std::endl;
        }

        std::vector<int> omega(cc_num, 0);
        z3::expr omega_phi_abs_i = m_phi_abs;
        z3::expr omega_phi_abs_i1(z3_ctx);
        int i = 0;
        listgraph omega_g_i = g_phi;
        listgraph omega_g_i1;
        do{
                i++;
                std::cout << "omega: [";
                for (int i=0; i<cc_num; i++)
                        std::cout << omega[i] << ", ";
                std::cout << "]\n";
                int j=0;
                bool is_feasible = true;
                if (cc_cycle_total_num > 0) {
                    get_omega_phi_abs(omega_phi_abs_i, omega_g_i, omega, m_phi_space, omega_phi_abs_i1);
                    omega_phi_abs_i = omega_phi_abs_i1;
                    // logger() << "omega phi abs: \n" << omega_phi_abs_i << std::endl;

                    z3::solver s(z3_ctx);
                    s.add(omega_phi_abs_i);

                    if (s.check() == z3::sat) {
                            // feasible allocating plans
                            std::cout << "feasible allocating plan!\n";
                            std::vector<std::set<int> > omega_eq_class_vec;
                            get_eq_class(omega_phi_abs_i, m_phi_const_vec, omega_eq_class_vec, m_phi_const_eq_class_vec);
                            m_omega_eq_size = omega_eq_class_vec.size();

                            for (int i=0; i<m_phi_const_eq_class_vec.size(); i++) {
                                std::cout << m_phi_const_vec[i] << ": " << m_phi_const_eq_class_vec[i] << std::endl;
                            }


                            m_phi_abs = omega_phi_abs_i;

                            construct_graph(m_phi_const_vec, m_phi_const_eq_class_vec, omega_eq_class_vec, m_phi_space, omega_g_i1);

                            std::string file_name = "omega_g_"+to_string(i)+"_"+to_string(j++)+".dot";
                            omega_g_i = omega_g_i1;
                            // omega_g_i.print(m_ctx.phi_const_vec, m_ctx.phi_space, file_name);
                            // exit(-1);
                            if (!omega_g_i.is_dag_like()) {
                                return check_allocating_plans(omega_g_i, g_psi, omega_phi_abs_i);
                            }

                            // match omega_g_i psi_g
                            // std::cout << "matching \n";
                            // match omega_graph to psi_graph
                            // m_ctx.phi_abs = omega_phi_abs_i;

                            // z3::expr phi = m_ctx.get_negf();
                            // m_ctx.phi_const_vec.clear(); // TODO. modify get_const_vec_and_eq_class
                            // m_ctx.phi_const_eq_class_vec.clear();
                            // int eq_size = get_const_vec_and_eq_class(phi, omega_phi_abs_i, m_ctx.phi_const_vec, m_ctx.phi_const_eq_class_vec);
                            // m_ctx.omega_eq_size = eq_size;
                            // logger() << "eq_to_eq table: \n";
                            /*
                            for (int i=0; i<m_ctx.psi_const_vec.size(); i++) {
                                    int psi_eq = m_ctx.psi_const_eq_class_vec[i];
                                    int idx = expr_tool::index_of_exp(m_ctx.psi_const_vec[i], m_ctx.phi_const_vec);
                                    int phi_eq = m_ctx.phi_const_eq_class_vec[idx];
                                    m_ctx.psi_eq_to_eq_table[psi_eq] = phi_eq;

                                    logger() << "psi_eq: " << psi_eq << "----" << "phi_eq: " << phi_eq << std::endl;
                            }
                            */
                    } else {
                        is_feasible = false;
                        std::cout << "unfeasible allocating plan!\n";
                    }
                }

                if (is_feasible) {
                    // match g_psi to omega_g_i
                    bool flag = match_graph(g_psi, omega_g_i);
                    // std::cout << "match flag: " << flag << std::endl;
                    // exit(-1);
                    if (!flag)  {
                            return false;
                    }
                }



                omega_g_i = g_phi;
                omega_phi_abs_i = phi_abs;
        } while(get_next_omega(omega, cc_cycle_num));

        return true;
}

/**
 * match pto
 * @param psi_atom : atom
 * @param omega_atom
 * @return true or false;
 */
bool listsolver::match_pto(z3::expr& psi_atom, z3::expr& omega_atom) {
        std::vector<z3::expr> psi_vars;
        std::vector<z3::expr> phi_atom_vars;
        psi_vars.push_back(psi_atom.arg(0));
        phi_atom_vars.push_back(omega_atom.arg(0));
        expr_tool::get_all_field_of_pto(psi_atom, psi_vars);
        expr_tool::get_all_field_of_pto(omega_atom, phi_atom_vars);
        z3::expr_vector eq_items(z3_ctx);
        // match
        for (int i=0; i<psi_vars.size(); i++) {
                z3::expr psi_v = psi_vars[i];
                z3::expr phi_v = phi_atom_vars[i];

                if (psi_v.to_string() != phi_v.to_string()) {
                    if (expr_tool::is_location(psi_v)) {
                        eq_items.push_back(z3_ctx.int_const(psi_v.to_string().c_str()) ==
                                           z3_ctx.int_const(phi_v.to_string().c_str()));
                    } else {
                        eq_items.push_back(psi_v == phi_v);
                    }
                }


                // int phi_cls = m_ctx.phi_const_eq_class_vec[ expr_tool::index_of_exp(phi_v, m_ctx.phi_const_vec)];
                // int psi_cls = m_ctx.psi_const_eq_class_vec[expr_tool::index_of_exp(psi_v, m_ctx.psi_const_vec)];
                // if (m_ctx.psi_eq_to_eq_table[psi_cls] != phi_cls) return false;
        }


        if (eq_items.size() > 0) {
            z3::expr check_f = m_phi_abs && !z3::mk_and(eq_items);
            z3::solver s(z3_ctx);
            if (s.check() == z3::sat) {
                return false;
            }
        }
        return true;
}

/**
 * match path to atom space
 * @param paths : path atoms ids
 * @param atom_space : unfold space
 * @param omega_const_vec : omeg var vec
 * @param omega_const_eq_class_vec : omega eq_class
 * @param eq_to_eq table
 * @return true or false
 */
//bool listsolver::match_path_to_atom_space(std::vector<int> &paths, z3::expr &atom_space, std::vector<z3::expr>&  omega_const_vec, std::vector<int>&  omega_const_eq_class_vec, std::vector<int> &omega_eq_to_eq_table) {
//
//
//        for (int j=0; j<atom_space.num_args(); j++) {
//                z3::expr psi_j = atom_space.arg(j);
//                z3::expr phi_atom_j = m_phi_space.arg(paths[j]);
//                std::vector<z3::expr> psi_vars;
//                std::vector<z3::expr> phi_atom_vars;
//
//                if (expr_tool::is_fun(psi_j, "pto")) {
//                        // logger() << "atom_j: " << atom_j << std::endl;
//                        // logger() << "phi_atom_j: " << phi_atom_j << std::endl;
//                        psi_vars.push_back(psi_j.arg(0));
//                        phi_atom_vars.push_back(phi_atom_j.arg(0));
//                        expr_tool::get_all_field_of_pto(psi_j, psi_vars);
//                        expr_tool::get_all_field_of_pto(phi_atom_j, phi_atom_vars);
//                } else {
//                        for (int i=0; i<psi_j.num_args(); i++) {
//                                psi_vars.push_back(psi_j.arg(i));
//                                phi_atom_vars.push_back(phi_atom_j.arg(i));
//                        }
//                }
//                // match
//                for (int i=0; i<psi_vars.size(); i++) {
//                        z3::expr psi_v = psi_vars[i];
//                        z3::expr phi_v = phi_atom_vars[i];
//
//                        //std::cout << "psi_v: " << psi_v << ", phi_v: " << phi_v << std::endl;
//
//                        int omega_cls = omega_const_eq_class_vec[expr_tool::index_of_exp(psi_v, omega_const_vec)];
//                        int phi_cls = m_phi_const_eq_class_vec[ expr_tool::index_of_exp(phi_v, m_phi_const_vec)];
//                        if (omega_eq_to_eq_table[omega_cls] != -1 && omega_eq_to_eq_table[omega_cls] != phi_cls) {
//                                //std::cout << "omega return false.\n";
//                                return false;
//                        }
//                        omega_eq_to_eq_table[omega_cls] = phi_cls;
//                        // phi
//                        int psi_idx = expr_tool::index_of_exp(psi_v, m_psi_const_vec);
//                        int psi_cls = m_psi_const_eq_class_vec[psi_idx];
//                        //std::cout << "psi class: " << psi_cls<<", phi class: "<< phi_cls << std::endl;
//                        if (psi_idx != -1 && m_psi_eq_to_eq_table[psi_cls] != phi_cls) {
//                                //std::cout << "global return false;\n";
//                                return false;
//                        }
//                }
//        }
//        return true;
//}

/**
 * match graph psi to omega_graph
 * @param g_psi
 * @param omega_g_i
 * @return true or false
 */
bool listsolver::match_graph(listgraph& g_psi, listgraph& omega_g_i) {

        //  get psi_edges
        std::vector<std::pair<std::pair<int, int>, int> > psi_edge_vec;
        g_psi.get_edges(psi_edge_vec);
        // get omega_edges
        std::vector<std::pair<std::pair<int, int>, int> > omega_edge_vec;
        omega_g_i.get_edges(omega_edge_vec);
        // std::vector<int> omega_edge_table(omega_edge_vec.size(), -1);
        std::map<int, int> omega_edge_table;
        for (int i=0; i<omega_edge_vec.size(); i++) {
                omega_edge_table[omega_edge_vec[i].second] = -1;
        }

        //CC
        std::vector<int> cc_cycle_num = omega_g_i.get_cc_cycle_num();
        std::vector<std::pair<int, int> > cc_cycle_table;
        for (int i=0; i<cc_cycle_num.size(); i++) {
                std::pair<int, int> selected(-1, -1);
                if (cc_cycle_num[i] > 0) {
                        selected.first = 0; // -1: no cycle; 0: no process; 1: candidate  2:selected
                }
                cc_cycle_table.push_back(selected);
        }

        std::cout << std::endl;
        std::vector<z3::expr> psi_const_vec = m_psi_const_vec;
        std::vector<z3::expr> phi_const_vec = m_phi_const_vec;
        z3::expr psi_space = m_psi_space;
        z3::expr phi_space = m_phi_space;

        std::pair<std::pair<int, int>, int> edge;
        // for each edge match one path in omega_graph
        for (int i=0; i<psi_edge_vec.size(); i++) {
                std::cout <<"psi edge: "<< m_psi_const_vec[psi_edge_vec[i].first.first] << "--" << psi_edge_vec[i].second << "--" << m_psi_const_vec[psi_edge_vec[i].first.second] << std::endl;

                edge = psi_edge_vec[i];
                z3::expr psi_atom = psi_space.arg(edge.second);
                int src = expr_tool::index_of_exp(psi_const_vec[edge.first.first], phi_const_vec);
                int dst = expr_tool::index_of_exp(psi_const_vec[edge.first.second], phi_const_vec);


                src = omega_g_i.get_vertex_id(src);
                dst = omega_g_i.get_vertex_id(dst);

                // std::cout << "src: " << src << ", dst: " << dst << std::endl;


                // src and dst are in the same cc
                int cc_id = omega_g_i.which_cc(src);
                if (cc_id != omega_g_i.which_cc(dst)) return false; // path in different cc

                std::vector<listgraph::edge_descriptor> path = omega_g_i.get_path(src, dst);

                std::cout << "path: \n";
                for (int j=0; j<path.size(); j++) {
                        std::cout << omega_g_i.source(path[j]);
                        std::cout << "---";
                        std::cout << omega_g_i.get_edge_property(path[j]);
                        std::cout << "---";
                        std::cout << omega_g_i.target(path[j]) << std::endl;

                }

                std::vector<int> paths;
                for (int j=0; j<path.size(); j++) {
                        paths.push_back(omega_g_i.get_edge_property(path[j]));
                }

                // special case
                if (paths.size()==0 && src!=dst) return false;

                int edge_num = paths.size();
                if (expr_tool::is_fun(psi_atom, "pto")) {
                        // pto match pto
                        if (edge_num==1) {
                                z3::expr omega_phi_atom = phi_space.arg(paths[0]);
                                if (!expr_tool::is_fun(omega_phi_atom, "pto")) {
                                        return z3::unsat;
                                }
                                // match pto atom
                                if(!match_pto(psi_atom, omega_phi_atom)) return false;
                        } else {
                                return false;
                        }
                } else {
                        // pred_atom match path
                        if (cc_cycle_num[cc_id] == 0) {
                                std::cout << "omega graph is dag. \n";
                                // dag
                                if(!match_path_to_atom_space(paths, psi_atom)) return false;
                        } else {
                            std::cout << "omega graph is dag-like. \n";
                                // dag_like (each cc has at most one cycle)
                                std::pair<int, int> coord(cc_id, 0);
                                std::vector<int> cycle = omega_g_i.get_cycle(coord);

                                std::vector<listgraph::edge_t> edge_cycle = omega_g_i.get_edge_cycle(coord);

                                std::cout << "paths size: " << paths.size() << std::endl;
                                // match paths
                                bool match_res1 = match_path_to_atom_space(paths, psi_atom);
                                // match paths+cycle
                                std::vector<listgraph::edge_descriptor> cycle_path = omega_g_i.get_path(dst);
                                int left_end = -1;
                                for (int j=0; j<edge_cycle.size(); j++) {
                                    if (left_end == -1) {
                                        if (edge_cycle[j].first.first == dst) {
                                            left_end = j;
                                            paths.push_back(edge_cycle[j].second);
                                        }
                                    } else {
                                        paths.push_back(edge_cycle[j].second);
                                    }
                                }
                                for (int j=0; j<left_end; j++) {
                                    paths.push_back(edge_cycle[j].second);
                                }
                                for (int j=0; j<edge_cycle.size(); j++) {
                                        std::cout <<"cycle edge: "<< edge_cycle[j].first.first << "--" << (edge_cycle[j].second) << "--" << edge_cycle[j].first.second << std::endl;
                                }



                                bool match_res2 = true;

                                if (edge_cycle.size() > 0) {
                                    match_res2 = match_path_to_atom_space(paths, psi_atom);
                                }

                                std::cout << "match_res1: " << match_res1 << ", match_res2: " << match_res2 << std::endl;

                                // whether dst and last_src are in cycle
                                int last_src = -1;
                                if (path.size() > 0) {
                                        last_src = omega_g_i.source(path[path.size()-1]);
                                }
                                if (index_of_int(dst, cycle) != -1 && index_of_int(last_src, cycle) == -1) {
                                        // dst in, last_src not in
                                        if (cc_cycle_table[cc_id].first != 2) {
                                                if (match_res1 && match_res2) {
                                                        cc_cycle_table[cc_id].first = 1;
                                                        cc_cycle_table[cc_id].second = i;
                                                } else if (!match_res1 && match_res2) {
                                                        cc_cycle_table[cc_id].first = 2;
                                                        cc_cycle_table[cc_id].second = i;
                                                        edge_num = paths.size(); // add edges
                                                } else if (!match_res1 && !match_res2) {
                                                        return false;
                                                }
                                        } else {
                                                if (!match_res1) return false;
                                        }
                                } else if(index_of_int(dst, cycle) != -1 && src==dst){
                                        // dst in, last_src in, src == dst
                                        if (cc_cycle_table[cc_id].first != 2 && match_res2) {
                                                cc_cycle_table[cc_id].first = 1;
                                                cc_cycle_table[cc_id].second = i;
                                        }
                                } else {
                                        if (!match_res1) return false;
                                        if (index_of_int(dst, cycle) != -1) {
                                                // dst in cycle
                                                cc_cycle_table[cc_id].first = 2;
                                        }
                                }
                        }
                }

                for (int j=0; j<edge_num; j++) {
                        omega_edge_table[paths[j]] ++;
                }
        }

        // check global info
        std::cout <<"cc_cycle_table: \n";
        for (int i=0; i<cc_cycle_table.size(); i++) {

                std::cout << "cc_cycle_num: " << i << " , status: " << cc_cycle_table[i].first << std::endl;

                if (cc_cycle_table[i].first==0) return false;
                if (cc_cycle_table[i].first==1) {
                        std::pair<int, int> coord(i, 0);
                        std::vector<int> cycle = omega_g_i.get_cycle(coord);
                        std::vector<listgraph::edge_descriptor> cycle_path = omega_g_i.get_path(cycle[0]);
                        for (int j=0; j<cycle_path.size(); j++) {
                                int path_idx = omega_g_i.get_edge_property(cycle_path[j]);
                                omega_edge_table[path_idx]++;
                        }
                }
        }

        //
        std::cout << "omega_edges_table: \n";

        std::map<int, int>::iterator iter;
        int i=0;
        for (iter=omega_edge_table.begin(); iter!=omega_edge_table.end(); iter++, i++) {
                std::cout << "edge: " << i <<", property: " <<iter->first << " , status: " << iter->second << std::endl;
                if (iter->second!=0) return false;
        }
        return true;
}


bool listsolver::match_path_to_atom_space(std::vector<int> &paths, z3::expr &psi_atom) {

        // path match pred atom
        if (paths.size() == 0) {
                // abs_omega_phi |= pred atom is empty
                z3::expr entl_empty_f = m_phi_abs;
                z3::expr eq_atom = z3_ctx.bool_val(true);
                Predicate_SLID_INT *pred = dynamic_cast<Predicate_SLID_INT *>(m_problem->getPredicate()); 
                int st_size = pred->size_of_static_parameters();
                int pars_size = psi_atom.num_args()-st_size;
                for (int i=0; i<pars_size/2; i++) {
                        eq_atom = eq_atom && expr_tool::eq_exp(z3_ctx, psi_atom.arg(i), psi_atom.arg(i+pars_size/2));
                }
                entl_empty_f = entl_empty_f && !eq_atom;
                z3::solver s(z3_ctx);
                s.add(entl_empty_f);
                if (s.check() == z3::unsat) return true;
                else return false;
        } else {
            // generate conditions
            z3::expr_vector data_items(z3_ctx);
            z3::expr_vector left_data_items(z3_ctx);
            left_data_items.push_back(z3_ctx.bool_val(true));
            int counter = 0;

            Predicate_SLID_INT *pred = dynamic_cast<Predicate_SLID_INT *>(m_problem->getPredicate()); 
            z3::expr phi_p = pred->get_phi_p();
            std::cout << "phi_p: " << phi_p << std::endl;
            z3::expr_vector data_pars(z3_ctx);
            pred->get_data_pars(data_pars);
            std::cout << "data pars: " << data_pars << std::endl;

            int psi_arg_size = psi_atom.num_args();
            if (paths.size() == 1) {
                // pred -> pto or pred
                z3::expr phi_atom = m_phi_space.arg(paths[0]);

                if (expr_tool::is_fun(phi_atom, "pto")) {
                    // left is pto
                    std::vector<z3::expr> phi_vars;
                    expr_tool::get_all_field_of_pto(phi_atom, phi_vars);
                    int expected_size = psi_arg_size/2;
                    if (phi_vars.size() < expected_size) {
                        for (int j=phi_vars.size(); j<expected_size; j++) {
                            std::string new_data_name = "_data_"+to_string(counter++);
                            z3::expr new_data_var = z3_ctx.int_const(new_data_name.c_str());
                            phi_vars.push_back(new_data_var);
                            left_data_items.push_back(new_data_var == psi_atom.arg(j));
                        }
                    }



                    z3::expr_vector pars(z3_ctx);
                    for (int i=1; i<phi_vars.size(); i++) {
                        if (!expr_tool::is_location(phi_vars[i])) {
                            pars.push_back(phi_vars[i]);
                        }
                        if (phi_vars[i].to_string() == psi_atom.arg(i).to_string()) {
                            continue;
                        }
                        if (expr_tool::is_location(phi_vars[i])) {
                            z3::expr int_var1 = z3_ctx.int_const(phi_vars[i].to_string().c_str());
                            z3::expr int_var2 = z3_ctx.int_const(psi_atom.arg(i).to_string().c_str());
                            data_items.push_back(int_var1 == int_var2);
                        } else {
                            data_items.push_back(phi_vars[i] == psi_atom.arg(i));
                        }
                    }
                    // phi_p data item
                    for (int i=expected_size; i<psi_arg_size; i++) {
                        if (!expr_tool::is_location(psi_atom.arg(i))) {
                            pars.push_back(psi_atom.arg(i));
                        }
                    }

                    data_items.push_back(phi_p.substitute(data_pars, pars));

                    // logger() << "data_items: " << data_items << std::endl;
                    // exit(-1);


                } else {
                    // pred
                    std::cout << "pred -> pred: \n";
                    z3::expr phi_atom = m_phi_space.arg(paths[0]);
                    for (int i=0; i<psi_arg_size; i++) {
                        if (phi_atom.arg(i).to_string() == psi_atom.arg(i).to_string()) {
                            continue;
                        }
                        if (expr_tool::is_location(psi_atom.arg(i))) {
                            z3::expr int_var1 = z3_ctx.int_const(phi_atom.arg(i).to_string().c_str());
                            z3::expr int_var2 = z3_ctx.int_const(psi_atom.arg(i).to_string().c_str());
                            data_items.push_back(int_var1 == int_var2);
                        } else {
                            data_items.push_back(phi_atom.arg(i) == psi_atom.arg(i));
                        }
                    }

                }
            } else {

                std::vector<z3::expr> head_vec;
                std::vector<z3::expr> cache_vec;
                for (int i=0; i<paths.size()-1; i++) {
                    z3::expr atom1 = m_phi_space.arg(paths[i]);
                    z3::expr atom2 = m_phi_space.arg(paths[i+1]);
                    std::vector<z3::expr> var_vec1;
                    std::vector<z3::expr> var_vec2;

                    bool is_pto1 = expr_tool::is_fun(atom1, "pto");
                    bool is_pto2 = expr_tool::is_fun(atom2, "pto");
                    // get var_vec
                    if (is_pto1) {
                        if (cache_vec.size() == 0) {
                            expr_tool::get_all_field_of_pto(atom1, var_vec1);
                            int expected_size = psi_arg_size/2;
                            if (var_vec1.size() < expected_size) {
                                for (int j=var_vec1.size(); j<expected_size; j++) {
                                    std::string new_data_name = "_data_"+to_string(counter++);
                                    z3::expr new_data_var = z3_ctx.int_const(new_data_name.c_str());
                                    var_vec1.push_back(new_data_var);
                                    if (i == 0) {
                                        left_data_items.push_back(new_data_var == psi_atom.arg(j));
                                    }

                                }
                            }
                        } else {
                            var_vec1 = cache_vec;
                        }
                        if (i == 0) {
                            head_vec = var_vec1;
                        }
                    } else {
                        for (int j=atom1.num_args()/2; j<atom1.num_args(); j++) {
                            var_vec1.push_back(atom1.arg(j));
                        }
                        if (i == 0) {
                            for (int j=0; j<atom1.num_args()/2; j++) {
                                head_vec.push_back(atom1.arg(j));
                            }
                        }
                    }



                    if (is_pto2) {
                        expr_tool::get_all_field_of_pto(atom2, var_vec2);
                        int expected_size = psi_arg_size/2;
                        if (var_vec2.size() < expected_size) {
                            for (int j=var_vec2.size(); j<expected_size; j++) {
                                std::string new_data_name = "_data_"+to_string(counter++);
                                z3::expr new_data_var = z3_ctx.int_const(new_data_name.c_str());
                                var_vec2.push_back(new_data_var);
                                if(!is_pto1) {
                                    left_data_items.push_back(new_data_var == var_vec1[j]);
                                }
                            }
                        }
                        cache_vec = var_vec2;
                    } else {
                        for (int j=0; j<atom2.num_args()/2; j++) {
                            var_vec2.push_back(atom2.arg(j));
                        }
                        cache_vec.clear();
                    }
                    std::cout << "atom1: " << atom1 << std::endl;
                    std::cout << "atom2: " << atom2 << std::endl;
                    std::cout << "vec1:  ";
                    for (int j=0; j<var_vec1.size(); j++) std::cout << var_vec1[j] << " ";
                    std::cout << std::endl;
                    std::cout << "vec2:  ";
                    for (int j=0; j<var_vec2.size(); j++) std::cout << var_vec2[j] << " ";
                    std::cout << std::endl;
                    z3::expr_vector pars(z3_ctx);
                    if (is_pto1) {
                        for (int j=1; j<var_vec1.size(); j++) {
                            if (!expr_tool::is_location(var_vec1[j])) {
                                pars.push_back(var_vec1[j]);
                            }
                        }
                        for (int j=1; j<var_vec2.size(); j++) {
                            if (!expr_tool::is_location(var_vec2[j])) {
                                pars.push_back(var_vec2[j]);
                            }
                        }
                        z3::expr data_item = phi_p.substitute(data_pars, pars);
                        data_items.push_back(data_item);


                        if (is_pto2) {
                            left_data_items.push_back(data_item);
                        }
                        if (var_vec2.size()>=2 && expr_tool::is_location(var_vec2[1])) {
                                // dll
                              data_items.push_back(z3_ctx.int_const(atom1.arg(0).to_string().c_str()) ==
                                                 z3_ctx.int_const(var_vec2[1].to_string().c_str()));
                        }

                    } else {
                        // atom1 is pred
                        for (int j2=var_vec2.size()-1, j1=var_vec1.size()-1; j2>=1; j2--, j1--) {
                            std::cout << var_vec1[j1] << " " << var_vec2[j2] << std::endl;
                            if (var_vec2[j2].to_string() == var_vec1[j1].to_string()) {
                                continue;
                            }
                            if (expr_tool::is_location(var_vec2[j2])) {
                                z3::expr int_var1 = z3_ctx.int_const(var_vec2[j2].to_string().c_str());
                                z3::expr int_var2 = z3_ctx.int_const(var_vec1[j1].to_string().c_str());
                                data_items.push_back(int_var1 == int_var2);
                            } else {
                                data_items.push_back(var_vec2[j2] == var_vec1[j1]);
                            }
                        }
                    }
                }
                // head and tail
                z3::expr atom2 = m_phi_space.arg(paths[paths.size()-1]);
                std::vector<z3::expr> var_vec2;
                bool is_pto2 = expr_tool::is_fun(atom2, "pto");

                 for (int j2=head_vec.size()-1, j1=psi_atom.num_args()/2-1; j2>=1; j2--, j1--) {
                        std::cout << head_vec[j2] << " " << psi_atom.arg(j1) << std::endl;
                        if (head_vec[j2].to_string() == psi_atom.arg(j1).to_string()) {
                            continue;
                        }
                        if (expr_tool::is_location(head_vec[j2])) {
                            z3::expr int_var1 = z3_ctx.int_const(head_vec[j2].to_string().c_str());
                            z3::expr int_var2 = z3_ctx.int_const(psi_atom.arg(j1).to_string().c_str());
                            data_items.push_back(int_var1 == int_var2);
                        } else {
                            data_items.push_back(head_vec[j2] == psi_atom.arg(j1));
                        }
                }
                if (is_pto2) {
                    if (cache_vec.size() == 0) {
                        expr_tool::get_all_field_of_pto(atom2, var_vec2);
                        int expected_size = psi_arg_size/2;
                        if (var_vec2.size() < expected_size) {
                            for (int j=var_vec2.size(); j<expected_size; j++) {
                                std::string new_data_name = "_data_"+to_string(counter++);
                                z3::expr new_data_var = z3_ctx.int_const(new_data_name.c_str());
                                var_vec2.push_back(new_data_var);
                                // left_data_items.push_back(new_data_var == psi_atom.arg(psi_arg_size/2+j));
                            }
                        }
                    } else {
                        var_vec2 = cache_vec;
                    }

                } else {

                    for (int j=atom2.num_args()/2; j<atom2.num_args(); j++) {
                        var_vec2.push_back(atom2.arg(j));
                    }
                }

                if (is_pto2) {
                    z3::expr_vector pars(z3_ctx);
                    for (int j=0; j<var_vec2.size(); j++) {
                        if (!expr_tool::is_location(var_vec2[j])){
                            pars.push_back(var_vec2[j]);
                        }
                    }
                    for (int j=psi_arg_size/2; j<psi_arg_size; j++) {
                        if (!expr_tool::is_location(psi_atom.arg(j))){
                            pars.push_back(psi_atom.arg(j));
                        }
                    }
                    data_items.push_back(phi_p.substitute(data_pars, pars));

                } else {
                    for (int j2=var_vec2.size()-1, j1=psi_arg_size-1; j2>=1; j2--, j1--) {
                            std::cout << var_vec2[j2] << " " << psi_atom.arg(j1) << std::endl;
                            if (var_vec2[j2].to_string() == psi_atom.arg(j1).to_string()) {
                                continue;
                            }
                            if (expr_tool::is_location(var_vec2[j2])) {
                                z3::expr int_var1 = z3_ctx.int_const(var_vec2[j2].to_string().c_str());
                                z3::expr int_var2 = z3_ctx.int_const(psi_atom.arg(j1).to_string().c_str());
                                data_items.push_back(int_var1 == int_var2);
                            } else {
                                data_items.push_back(var_vec2[j2] == psi_atom.arg(j1));
                            }
                    }
                }
            }
            std::cout << "left data items: " << left_data_items << std::endl;
            std::cout << "data_items: " << data_items << std::endl;
            // exit(-1);
            if (data_items.size() > 0) {
                z3::expr check_f = m_phi_abs && z3::mk_and(left_data_items) && !z3::mk_and(data_items);
                z3::solver s(z3_ctx);
                s.add(check_f);
                if (s.check() == z3::sat) {
                    return false;
                }
                // logger() << "check_f: " << check_f << std::endl;
                // exit(-1);
            }
            return true;
        }

        /*
        // 1. psi_pred_atom - extend by path
        z3::expr omega_f(z3_ctx());
        z3::expr atom_space(z3_ctx());
        // z3::expr_vector new_vars(z3_ctx());
        unfold_by_path(paths, psi_atom, omega_f);
        z3::expr omega_f_abs(z3_ctx());
        // logger() << "omega_f: " << omega_f << std::endl;
        get_data_space(omega_f, omega_f_abs, atom_space);
        // omega_f_abs = get_abstraction(omega_f, atom_space, new_bools);
        logger() << "omega_f_abs data: " << omega_f_abs << std::endl;
        omega_f_abs = omega_f_abs && m_ctx.psi_abs;
        // logger() << "omega_f_abs: " << omega_f_abs <<std::endl;
        std::set<z3::expr, exprcomp> omega_const_set;
        expr_tool::get_consts(omega_f, omega_const_set);
        std::vector<z3::expr> oemga_const_vec;
        std::vector<int> omega_const_eq_class_vec(omega_const_set.size(), -1);
        int eq_count = get_const_vec_and_eq_class(omega_f, omega_f_abs, oemga_const_vec, omega_const_eq_class_vec);
        logger() << "eq_count: " << eq_count << std::endl;
        std::vector<int> omega_eq_to_eq_table(eq_count, -1);

        if(!match_path_to_atom_space(paths, atom_space, oemga_const_vec, omega_const_eq_class_vec, omega_eq_to_eq_table)) return false;
        return true;
        */
}


/**
 * unfold predicate atom by path
 * @param path : match path
 * @param psi_atom : pred atom
 * @param omega_f : unfold formula
 */
void listsolver::unfold_by_path(std::vector<int> &path, z3::expr &psi_atom, z3::expr &omega_f) {
        z3::expr atom_data = z3_ctx.bool_val(true);
        std::vector<z3::expr> atom_spaces;
        atom_spaces.push_back(psi_atom);
        Predicate_SLID_INT *pred = dynamic_cast<Predicate_SLID_INT *>(m_problem->getPredicate());  // entl problem has only one predicate
        int st_par_size = pred->size_of_static_parameters();

        // unfold_by_path(path, psi_atom, atom_data, atom_space);
        for (int j=0; j<path.size(); j++) {
                z3::expr atom_j = m_phi_space.arg(path[j]);
                // logger() << "atom_j: " << atom_j << std::endl;
                if (expr_tool::is_fun(atom_j, "pto")) {
                        // extend one pto
                        unfold_pto(pred, atom_data, atom_spaces);
                        if (j==path.size()-1) {
                                z3::expr last_pred = atom_spaces.back();
                                int size = last_pred.num_args() - st_par_size;
                                for(int k=0; k<size/2; k++) {
                                        atom_data = atom_data && (last_pred.arg(k) == last_pred.arg(k+size/2));
                                }
                                atom_spaces.pop_back();
                        }
                } else if (j<path.size()-1) {
                        // extend one pred
                        unfold_pred(pred, atom_spaces);
                }
        }

        // make a space atom
        z3::expr atom_space = atom_spaces[0];
        if (atom_spaces.size() >= 2) {
                z3::sort range = z3_ctx.bool_sort();
                z3::sort_vector domains(z3_ctx);
                for (unsigned i=0; i<atom_spaces.size(); i++) {
                        domains.push_back(range);
                }
                z3::func_decl sep_f = z3_ctx.function("sep", domains, range);
                z3::expr_vector args(z3_ctx);
                for (int j=0; j<atom_spaces.size(); j++) args.push_back(atom_spaces[j]);
                atom_space = sep_f(args);
        }

//        z3::sort domain = z3_ctx.uninterpreted_sort("bool");
//        z3::sort range = z3_ctx().bool_sort();
//        z3::func_decl tobool_f = z3_ctx().function("tobool", domain, range);
//        atom_space = tobool_f(atom_space);
        omega_f = atom_data && atom_space;
}



/**
 * get const vector and eq_class by phi and phi_abs
 * @param phi : the formula phi
 * @param phi_abs : the abstraction of phi
 * @param const_vec : output const vector
 * @param const_eq_class : output const vector map eq_class
 * @return int: the eq_class_count
 */
int listsolver::get_const_vec_and_eq_class(z3::expr &phi, z3::expr phi_abs, std::vector<z3::expr> &const_vec, std::vector<int> &const_eq_class) {
        std::set<z3::expr, exprcomp> phi_lconst_set;
        expr_tool::get_lconsts(phi, phi_lconst_set);
        std::vector<z3::expr> phi_lconst_vec;
        phi_lconst_vec.push_back(z3_ctx.int_const("nil"));
        expr_tool::expr_set_to_vec(phi_lconst_set, phi_lconst_vec);
        std::vector<std::set<int> > phi_location_eq_class_vec;
        std::vector<int> phi_lconst_class(phi_lconst_vec.size(), -1);
        get_eq_class(phi_abs, phi_lconst_vec, phi_location_eq_class_vec, phi_lconst_class);
        // get data eq_class_vec
        std::set<z3::expr, exprcomp> phi_dconst_set;
        expr_tool::get_dconsts(phi, phi_dconst_set);
        std::vector<z3::expr> phi_dconst_vec;
        expr_tool::expr_set_to_vec(phi_dconst_set, phi_dconst_vec);
        std::vector<std::set<int> > phi_data_eq_class_vec;
        std::vector<int> phi_dconst_class(phi_dconst_vec.size(), -1);
        get_eq_class(phi_abs, phi_dconst_vec, phi_data_eq_class_vec, phi_dconst_class);

        // logger() << "var size in get: " << phi_lconst_vec.size() + phi_dconst_vec.size() << std::endl;


        int loc_size = phi_lconst_vec.size();
        int data_size = phi_dconst_vec.size();
        init_int_vector(const_eq_class, loc_size+data_size);
        int loc_eq_size = phi_location_eq_class_vec.size();
        for (int i=0; i<phi_lconst_vec.size(); i++) {
                const_vec.push_back(phi_lconst_vec[i]);
                const_eq_class[i] = phi_lconst_class[i];
                // logger() << phi_lconst_vec[i] << std::endl;
        }
        for (int i=0; i<data_size; i++) {
                const_vec.push_back(phi_dconst_vec[i]);
                const_eq_class[loc_size+i] = phi_dconst_class[i]+loc_eq_size;
                // logger() << phi_dconst_vec[i] << std::endl;
        }

        // logger() << "location eq class :" << phi_location_eq_class_vec.size() << std::endl;
        // logger() << "data eq class :" <<phi_data_eq_class_vec.size() << std::endl;


        return phi_location_eq_class_vec.size() + phi_data_eq_class_vec.size();
}



/**
 * extend predicate atom
 * @param pred : predicate definition
 * @parram atom_space : extend space atoms
 */
void listsolver::unfold_pred(Predicate_SLID_INT *pred, std::vector<z3::expr> &atom_space) {

        int unfold_idx = atom_space.size();
        z3::expr atom = atom_space[unfold_idx-1];
        int size = atom.num_args() - pred->size_of_static_parameters(); // size of source and destination paramaters
        atom_space.pop_back();
        z3::expr_vector pred1_args(z3_ctx);
        z3::expr_vector pred2_args(z3_ctx);
        for (int i=0; i<size/2; i++) {
                pred1_args.push_back(atom.arg(i));
                z3::sort st = atom.arg(i).get_sort();
                std::string new_name = "pred_var_"+to_string(i)+"_"+to_string(unfold_idx);
                z3::expr new_var = z3_ctx.constant(new_name.c_str(), st);
                // new_vars.push_back(new_var);
                pred2_args.push_back(new_var);
        }
        for (int i=size/2; i<size; i++) {
                pred1_args.push_back(pred2_args[i-size/2]);
                pred2_args.push_back(atom.arg(i));
        }
        for (int i=size; i<atom.num_args(); i++) {
                pred1_args.push_back(atom.arg(i));
                pred2_args.push_back(atom.arg(i));
        }
        z3::func_decl pred_fun = pred->get_fun();
        z3::expr pred1 = pred_fun(pred1_args);
        z3::expr pred2 = pred_fun(pred2_args);
        atom_space.push_back(pred1);
        atom_space.push_back(pred2);
}

/**
 * extend one pto atom
 * @param pred : predicate definition
 * @param atom_data : extend data constraints
 * @param atom_space : extend space atoms
 */
void listsolver::unfold_pto(Predicate_SLID_INT *pred, z3::expr& atom_data, std::vector<z3::expr> &atom_space) {
        z3::expr data = pred->get_data();
        z3::expr pto = pred->get_pto();
        z3::expr rec_app = pred->get_rec_app();

        int unfold_idx = atom_space.size();
        z3::expr atom = atom_space[unfold_idx-1];
        atom_space.pop_back();

        z3::expr_vector args = pred->get_pars(); // predicate parameters, formal parameters
        z3::expr_vector f_args(z3_ctx);
        z3::expr_vector a_args(z3_ctx); // actual parameters

        // init formla parameters and actual parameters
        for (int j=0; j<atom.num_args(); j++) {
                f_args.push_back(args[j]);
                a_args.push_back(atom.arg(j));
        }

        z3::expr_vector x_h(z3_ctx);
        pred->get_x_h(x_h);
        // z3::expr_vector x_h_cons(z3_ctx());
        for (int j=0; j<x_h.size(); j++) {
                f_args.push_back(x_h[j]);
                z3::sort st = x_h[j].get_sort();
                std::string name = x_h[j].to_string();
                name = name.replace(name.find(":"), 1, "");
                name = name.replace(name.find(" "), 1, "_");
                std::string ss = name.substr(1, name.length()-2)+"_"+to_string(unfold_idx);
                z3::expr new_var = z3_ctx.constant(ss.c_str(), st);
                // new_vars.push_back(new_var);
                a_args.push_back(new_var);
        }


        data = data.substitute(f_args, a_args);
        atom_data = atom_data && data;
        pto = pto.substitute(f_args, a_args);
        rec_app = rec_app.substitute(f_args, a_args);
        atom_space.push_back(pto);
        atom_space.push_back(rec_app);
}


/**
 * get omega_phi_abs
 * @param phi_abs : last omega_phi_abs
 * @param g : last omega_graph
 * @param omega : new pseudo-plan
 * @param omega_phi_abs: new omega_phi_abs
 */
void listsolver::get_omega_phi_abs(z3::expr &phi_abs, listgraph &g, std::vector<int> &omega, z3::expr& space, z3::expr &omega_phi_abs) {
        std::vector<int> cc_cycle_num = g.get_cc_cycle_num();
        std::pair<int,int> coords;
        std::vector<listgraph::edge_t> cycle;
        omega_phi_abs = phi_abs;
        int cc_num = cc_cycle_num.size();
        for (int i=0; i<cc_num; i++) {
                int omega_i =omega[i];
                if (omega_i==0) {
                        //
                        for (int j=0; j<cc_cycle_num[i]; j++) {
                                coords.first = i;
                                coords.second = j;
                                cycle = g.get_edge_cycle(coords);
                                int cycle_size = cycle.size();
                                // edge
                                for (int k=0; k<cycle_size; k++) {
                                        listgraph::edge_t edge = cycle[k];
                                        int atom_idx = edge.second;
                                        z3::expr E = space.arg(atom_idx).arg(0);
                                        std::string E_bool_name = "["+E.to_string()+","+to_string(atom_idx)+"]";
                                        z3::expr E_bool = z3_ctx.bool_const(E_bool_name.c_str());
                                        omega_phi_abs = omega_phi_abs && (!E_bool);
                                }
                        }
                } else {
                        //
                        coords.first = i;
                        coords.second = omega_i-1;
                        cycle = g.get_edge_cycle(coords);
                        int cycle_size = cycle.size();
                        z3::expr zeta(z3_ctx);
                        // edge
                        for (int k=0; k<cycle_size; k++) {
                                listgraph::edge_t edge = cycle[k];
                                int atom_idx = edge.second;
                                z3::expr E = space.arg(atom_idx).arg(0);
                                std::string E_bool_name = "["+E.to_string()+","+to_string(atom_idx)+"]";
                                z3::expr E_bool = z3_ctx.bool_const(E_bool_name.c_str());

                                if (Z3_ast(zeta) == 0) {
                                        zeta = E_bool;
                                } else {
                                        zeta = zeta || E_bool;
                                }
                        }
                        omega_phi_abs = omega_phi_abs && zeta;
                }
        }
}

/**
 * construct_graph by phi
 * @param phi_abs : abstraction of phi
 * @param lconsts : the location vars
 * @param space : the space part
 * @param g : the output
 */
void listsolver::construct_graph(std::vector<z3::expr> &phi_const_vec, std::vector<int>& phi_const_eq_class_vec, std::vector<std::set<int> >& eq_class_vec, z3::expr &phi_space, listgraph &g) {
		// std::vector<std::set<int> > eq_class_vec;
        /*
        std::vector<z3::expr> phi_lconst_vec;
        for (int i=0; i<phi_const_vec.size(); i++) {
                if (!expr_tool::is_location(phi_const_vec[i])) break;
                phi_lconst_vec.push_back(phi_const_vec[i]);
        }*/
        // get_eq_class(phi_abs, phi_const_vec, eq_class_vec);


        // logger() << "size of eq_class_vec: " << eq_class_vec.size() << std::endl;

        std::vector<std::pair<std::pair<int, int>, int> > edge_vec;
        z3::solver s(z3_ctx);

        for (int i=0; i<phi_space.num_args(); i++) {
                std::pair<std::pair<int, int>, int> edge;
                edge.second = i;
                z3::expr atom = phi_space.arg(i);

                if (atom.decl().name().str()!="pto") {
                        // abs(\phi) \and [E,i]
                        z3::expr E = atom.arg(0);
                        //z3::expr F = atom.arg(atom.num_args()/2);
                        std::string bool_name = "["+E.to_string()+","+to_string(i)+"]";

                        z3::expr bool_var = z3_ctx.bool_const(bool_name.c_str());

                        s.reset();
                        s.add(m_phi_abs && bool_var);

                        if (s.check() == z3::unsat) {
                            continue;
                        }

                }
                get_edge_from_atom(atom, phi_const_vec, edge);
                edge_vec.push_back(edge);
        }

        // logger() << "size of edge_vec: " << edge_vec.size() << std::endl;
        g.init(eq_class_vec, edge_vec);
}

/**
 * get next omega, get next pseudo-plans
 * @param curr : current omega
 * @param target : target omega
 */
bool listsolver::get_next_omega(std::vector<int> &curr, std::vector<int> &target) {
		int size = curr.size();
        int c = 1;
        int i=size-1;

        while(c>0 && i>=0) {
                if (target[i] == 0) {
                        i--;
                } else {
                        if (curr[i]+c <= target[i]) {
							curr[i] = curr[i] + c;
							c = 0;
                        } else {
                        	curr[i] = (curr[i]+c) % (target[i]);
	                        c = 1;
                        }
                        i --;
                }
        }

        return c == 0;
}

/**
 * get edge from atom
 * @param atom : the atom
 * @param lconsts : the location var
 * @param edge : output
 */
void listsolver::get_edge_from_atom(z3::expr &atom, std::vector<z3::expr> &lconsts, std::pair<std::pair<int, int>, int> &edge) {
        Predicate_SLID_INT *pred = dynamic_cast<Predicate_SLID_INT *>(m_problem->getPredicate());
        //z3::expr plfld = pred.get_plfld();
        int size = pred->get_pars().size() - pred->size_of_static_parameters();
        z3::expr source(z3_ctx);
        z3::expr dest(z3_ctx);

        if (atom.decl().name().str()=="pto") {
//std::cout<<"get_edge begin0"<<std::endl;
                source = atom.arg(0);
                z3::expr sref = atom.arg(1);
                //if (sref.decl().name().str() == "sref") {
//                        for (int i=0; i<sref.num_args(); i++) {
//                                if (sref.arg(i).arg(0).to_string() == plfld.to_string()) {
//                                        dest = sref.arg(i).arg(1);
//                                        break;
//                                }
//                        }
                //} else {
                        dest = sref.arg(0);
                //}
        } else {
                source = atom.arg(0);
                dest = atom.arg(size/2);
        }
        edge.first.first = expr_tool::index_of_exp(source, lconsts);
        edge.first.second = expr_tool::index_of_exp(dest, lconsts);
}

/**
 * get equivalence class
 * @param: phi_abs : the abstraction of phi
 * @param: lconsts : the location vars
 * @param: eq_class_vec : the output
 */
void listsolver::get_eq_class(z3::expr &phi_abs, std::vector<z3::expr> &lconsts, std::vector<std::set<int> > &eq_class_vec) {
        std::vector<int> lconst_class(lconsts.size(), -1);
        get_eq_class(phi_abs, lconsts, eq_class_vec, lconst_class);
}

/**
 * get equivalence class
 * @param: phi_abs : the abstraction of phi
 * @param: lconsts : the location vars
 * @param: eq_class_vec : the output
 * @param: lconst_class : the output
 */
void listsolver::get_eq_class(z3::expr &phi_abs, std::vector<z3::expr> &lconsts, std::vector<std::set<int> > &eq_class_vec,  std::vector<int>& lconst_class) {
        std::set<int> eq_class;
        z3::solver sol(z3_ctx);
        // std::vector<int> lconst_class(lconsts.size(), -1);
        init_int_vector(lconst_class, lconsts.size());
        for (int i=0; i<lconsts.size(); i++) {
                for (int j=i+1; j<lconsts.size(); j++) {
                        z3::expr formula = phi_abs;
                        if (expr_tool::is_location(lconsts[i])){
                                z3::expr X = z3_ctx.int_const(lconsts[i].to_string().c_str());
                                z3::expr Y =  z3_ctx.int_const(lconsts[j].to_string().c_str());
                                formula = formula && (X!=Y);
                        } else {
                                formula = formula && (lconsts[i] != lconsts[j]);
                        }
                        sol.reset();
                        sol.add(formula);
                        // logger() << "eq class formula: " << formula << std::endl;
                        // logger() << "check sat: " << sol.check() << std::endl;
                        if(sol.check() == z3::unsat) {
                                // X == Y
                                if (lconst_class[i] != -1) {
                                        lconst_class[j] = lconst_class[i];
                                        eq_class_vec[lconst_class[i]].insert(j);
                                } else {
                                        lconst_class[i] = eq_class_vec.size();
                                        lconst_class[j] = lconst_class[i];
                                        eq_class.clear();
                                        eq_class.insert(i);
                                        eq_class.insert(j);
                                        eq_class_vec.push_back(eq_class);
                                }
                        } else {
                                // logger() << "model: " << sol.get_model() << std::endl;
                        }
                }
        }

        for (int i=0; i<lconsts.size(); i++) {
                if (lconst_class[i] == -1) {
                        lconst_class[i] = eq_class_vec.size();
                        eq_class.clear();
                        eq_class.insert(i);
                        eq_class_vec.push_back(eq_class);
                }
        }
}

/**
 * compute all predicate data  closure
 */
void listsolver::compute_all_data_closure() {
//    for (int i=0; i<m_ctx.pred_size(); i++) {
//                logger() << "compute predicate " << i << std::endl;
//                predicate pred = m_ctx.get_pred(i);
//                // 1. compute data closure (lfp)
//                z3::expr phi_pd_abs = compute_data_closure(pred);
//                logger() << "compute data closure: " << phi_pd_abs << std::endl;
//                delta_ge1_predicates.push_back(phi_pd_abs);
//        }
	Predicate_SLID_INT *pred = dynamic_cast<Predicate_SLID_INT*>(m_problem->getPredicate()); 
	z3::expr phi_pd_abs = compute_data_closure(pred);
	delta_ge1_predicate = phi_pd_abs;
}

/**
 * compute predicate data closure
 * @param pred: predicate
 * @return phi_pd : data closure
 */
z3::expr listsolver::compute_data_closure(Predicate_SLID_INT *pred) {
        z3::expr_vector args = pred->get_pars();
        z3::expr_vector xi(z3_ctx);
        z3::expr_vector alpha(z3_ctx);
        z3::expr_vector beta(z3_ctx);
        z3::expr_vector gamma(z3_ctx);
        z3::expr rec_app = pred->get_rec_app();

        int size = args.size();
        // 1. get xi
        int i=size-1;
        for (; i>=0; i--) {
                if (args[i].to_string().find("sta_")!=0) break;
                xi.push_back(args[i]);
        }
        size = i+1; // size of source and destination parameters
        // 2. get alpha beta
        for (i=1; i<size/2; i++) {
                alpha.push_back(args[i]);
                beta.push_back(args[i+size/2]);
                gamma.push_back(rec_app.arg(i));
        }

        // std::cout << "xi: " << xi << std::endl;
        // std::cout << "alpha: " << alpha << std::endl;
        // std::cout << "beta: " << beta << std::endl;
        // std::cout << "gamma: " << gamma << std::endl;

        std::vector<std::vector<std::vector<z3::expr>>> alpha_table;// alphai -> data items
        for (int i=0; i<alpha.size(); i++) {
                std::vector<std::vector<z3::expr>> data_items;
                for (int j=0; j<3; j++) {// 0:=, 1:<=, 2:>=
                        std::vector<z3::expr> data_item;
                        data_items.push_back(data_item);
                }
                alpha_table.push_back(data_items);
        }


        z3::expr data = pred->get_data();
        // std::cout << "data: " << data << std::endl;
        for (int i=0; i<data.num_args(); i++) {
                z3::expr data_item = data.arg(i);
                z3::expr normal_item = normalize_item(data_item);
                // std::cout << "data_item: " << normal_item << std::endl;
                z3::expr alpha_i = normal_item.arg(0);
                z3::expr right_i = normal_item.arg(1);
                int idx_i = index_of_vec(alpha_i, alpha);

                std::string opa = normal_item.decl().name().str();
                if (opa == "=") {
                        alpha_table[idx_i][0].push_back(normal_item);
                } else if (opa == "<=") {
                        alpha_table[idx_i][1].push_back(normal_item);
                } else {
                        alpha_table[idx_i][2].push_back(normal_item);
                }
        }


        /*
          for (int i=0; i<alpha.size(); i++) {
          std::cout << "alpha :" << alpha[i] << std::endl;
          for (int j=0; j<3; j++) {// 0:=, 1:<=, 2:>=
          std::cout << " " << alpha_table[i][j].size() << ":";
          for (int k=0; k<alpha_table[i][j].size(); k++) {
          std::cout << alpha_table[i][j][k] << " ";
          }
          std::cout << std::endl;
          }
          std::cout << std::endl;
          }
        */



        z3::expr phi_pd = z3_ctx.bool_val(true);
        for (int i=0; i<alpha.size(); i++) {
                std::vector<std::vector<z3::expr>> data_items = alpha_table[i];
                z3::expr gamma_i = gamma[i];
                z3::expr beta_i = beta[i];
                z3::expr closure_item = compute_alpha_closure(data_items, xi, gamma_i, beta_i);
                // std::cout << "closure item: " << closure_item << std::endl;
                phi_pd = phi_pd && closure_item;
        }
        return phi_pd;
}


/**
 * compute alpha_i closure item by related data items
 * @param data_items: alpha_i related data items
 * @param xi : static parameters
 * @param gamma :
 * @return closure data item
 */
z3::expr listsolver::compute_alpha_closure(std::vector<std::vector<z3::expr> > &data_items, z3::expr_vector &xi, z3::expr &gamma_i, z3::expr& beta_i) {
        z3::expr closure_item = z3_ctx.bool_val(true);
        z3::expr k = z3_ctx.int_const("k");

        // 1. =
        for (int i=0; i<data_items[0].size(); i++) {
                z3::expr item = data_items[0][i];
                // 1.1 alpha_i = c
                if (item.arg(1).num_args()!=2) {
                        closure_item = closure_item && item;
                } else {
                        z3::expr op1 = item.arg(1).arg(0);

                        int idx_j = index_of_vec(op1, xi);
                        // 1.2 alpha_i = xi_j + c
                        if (idx_j != -1) {
                                closure_item = closure_item && item;
                        }
                        // 1.3 alpha_i = gamma_i + c
                        else if (gamma_i.hash()==op1.hash()) {
                                int c = get_numeral(item.arg(1).arg(1));
                                closure_item = closure_item && (item.arg(0) == beta_i + k*c);
                        }
                }

        }
        // std::cout << "== closure item: " << closure_item << std::endl;
        // 2. <=
        z3::expr_vector le_cs(z3_ctx);
        z3::expr_vector le_xi_cs(z3_ctx);
        z3::expr_vector le_gamma_cs(z3_ctx);
        for (int i=0; i<data_items[1].size(); i++) {
                z3::expr item = data_items[1][i];
                // 1.1 alpha_i = c
                if (item.arg(1).num_args()!=2) {
                        closure_item = closure_item && item;
                        le_cs.push_back(item);
                } else {
                        z3::expr op1 = item.arg(1).arg(0);
                        int idx_j = index_of_vec(op1, xi);
                        // 1.2 alpha_i = xi_j + c
                        if (idx_j != -1) {
                                closure_item = closure_item && item;
                                le_xi_cs.push_back(item);
                        }
                        // 1.3 alpha_i = gamma_i + c
                        else if (gamma_i.hash()==op1.hash()) {

                                int c = get_numeral(item.arg(1).arg(1));
                                closure_item = closure_item && (item.arg(0) <= beta_i + k*c);
                                le_gamma_cs.push_back(item);
                        }
                }
        }

        for (int i=0; i<le_gamma_cs.size(); i++) {
                z3::expr item = le_gamma_cs[i];
                int c = get_numeral(item.arg(1).arg(1));
                if (c < 0) {
                        for (int j=0; j<le_cs.size();j++) {
                                z3::expr con_item = le_cs[j];
                                closure_item = closure_item && (item.arg(0) <= con_item.arg(1) + (k-1)*c);
                        }
                        for (int j=0; j<le_xi_cs.size();j++) {
                                z3::expr con_item = le_xi_cs[j];
                                closure_item = closure_item && (item.arg(0) <= con_item.arg(1) + (k-1)*c);
                        }
                }
        }
        // std::cout << "<= closure item: " << closure_item << std::endl;


        // 3. >=
        z3::expr_vector ge_cs(z3_ctx);
        z3::expr_vector ge_xi_cs(z3_ctx);
        z3::expr_vector ge_gamma_cs(z3_ctx);
        for (int i=0; i<data_items[2].size(); i++) {
                z3::expr item = data_items[2][i];
                // 1.1 alpha_i = c
                if (item.arg(1).num_args()!=2) {
                        closure_item = closure_item && item;
                        ge_cs.push_back(item);
                } else {
                        z3::expr op1 = item.arg(1).arg(0);
                        int idx_j = index_of_vec(op1, xi);
                        // 1.2 alpha_i = xi_j + c
                        if (idx_j != -1) {
                                closure_item = closure_item && item;
                                ge_xi_cs.push_back(item);
                        }
                        // 1.3 alpha_i = gamma_i + c
                        else if (gamma_i.hash()==op1.hash()) {

                                int c = get_numeral(item.arg(1).arg(1));
                                closure_item = closure_item && (item.arg(0) >= beta_i + k*c);
                                ge_gamma_cs.push_back(item);
                        }
                }
        }
        // std::cout << ">= closure item: " << closure_item << std::endl;

        for (int i=0; i<ge_gamma_cs.size(); i++) {
                z3::expr item = ge_gamma_cs[i];
                int c = get_numeral(item.arg(1).arg(1));
                if (c > 0) {
                        for (int j=0; j<ge_cs.size();j++) {
                                z3::expr con_item = ge_cs[j];
                                closure_item = closure_item && (item.arg(0) >= con_item.arg(1) + (k-1)*c);
                        }
                        for (int j=0; j<ge_xi_cs.size();j++) {
                                z3::expr con_item = ge_xi_cs[j];
                                closure_item = closure_item && (item.arg(0) >= con_item.arg(1) + (k-1)*c);
                        }
                }
        }
        // std::cout << ">= closure item: " << closure_item << std::endl;


        return closure_item;
}

int listsolver::index_of_vec(z3::expr x, z3::expr_vector &vec) {
        for (int i=0; i<vec.size(); i++)
                if (x.hash() == vec[i].hash()) return i;
        return -1;
}

/**
 * normalize data item: x > c -> x >= c+1 x<c -> x<c-1
 * @param x ; expr
 * @return normal expr
 */
z3::expr listsolver::normalize_item(z3::expr x) {
        std::string opa = x.decl().name().str();
        if (opa == ">") {
                if (x.arg(1).num_args()!=2) {
                        int c = get_numeral(x.arg(1))+1;
                        return x.arg(0) >= c;
                } else {
                        int c = get_numeral(x.arg(1).arg(1))+1;
                        return x.arg(0) >= x.arg(1).arg(0)+c;
                }
        }
        if (opa == "<") {
                if (x.arg(1).num_args()!=2) {
                        int c = get_numeral(x.arg(1))-1;
                        return x.arg(0) <= c;
                } else {
                        int c = get_numeral(x.arg(1).arg(1))-1;
                        return x.arg(0) <= x.arg(1).arg(0)+c;
                }
        }
        return x;
}

int listsolver::get_numeral(z3::expr x) {
        if (x.is_numeral()) {return x.get_numeral_int();}
        if (x.is_app() && x.decl().name().str()=="-" && x.num_args()==1 && x.arg(0).is_numeral()) return -x.arg(0).get_numeral_int();
        if (x.is_app()
            && (x.decl().name().str() == "to_real" || x.decl().name().str() == "to_int")) return get_numeral(x.arg(0));
        return 0;
}


/**
 * atom in formula to abstraction
 * @param  atom [the atom in formula, like p(Z1, mu; Z2, nu, chi) or (pto Z (*))]
 * @param  i    [the index in formula]
 * @param new_bools [new bool vars]
 * @return      [the abstraction]
 */
z3::expr listsolver::pred2abs(z3::expr &atom, int i, z3::expr_vector& new_bools) {
        //if (atom.to_string() == "emp") return z3_ctx().bool_val(true);

        std::string source = atom.arg(0).to_string();
        std::string new_name = "["+source+","+to_string(i)+"]";
        // 1 introduce new vars
        z3::expr source_bool = z3_ctx.bool_const(new_name.c_str()); // [Z1,i]
        new_bools.push_back(source_bool);
        z3::expr source_int = z3_ctx.int_const(source.c_str()); // Z1

        z3::expr atom_f(z3_ctx);
        if (expr_tool::is_fun(atom, "pto")) {
                // 1.1 pto atom
                atom_f = (source_bool && source_int > 0);
        } else {
                z3::expr or_0 = pred_unfold_0(atom, i, new_bools);
                z3::expr or_ge1 = pred_unfold_ge1(atom, i, new_bools);
                // 1.3 or
                atom_f = or_0 || or_ge1;
        }
        return atom_f;
}


z3::expr listsolver::pred_unfold_0(z3::expr& atom, int i, z3::expr_vector& new_bools) {
        std::string source = atom.arg(0).to_string();
        std::string new_name = "["+source+","+to_string(i)+"]";
        //std::string pred_name = atom.decl().name().str();
        //int index = 0;//index_of_pred(pred_name);
        //predicate pred = m_problem.get_pred(index); // get predicate definition
        Predicate_SLID_INT *pred = dynamic_cast<Predicate_SLID_INT*>(m_problem->getPredicate());
        int size = atom.num_args() - pred->size_of_static_parameters(); // size of source and destination paramaters
        z3::expr source_bool = z3_ctx.bool_const(new_name.c_str()); // [Z1,i]
        new_bools.push_back(source_bool);
        z3::expr source_int = z3_ctx.int_const(source.c_str()); // Z1

        // 1.2 predicate atom
        // 1.2.1 supposing atom is empty
        std::string k_name = "[k,"+to_string(i)+"]";
        z3::expr k_i_int = z3_ctx.int_const(k_name.c_str()); // k_i

        z3::expr or_0(z3_ctx);
        z3::expr dest_int = z3_ctx.int_const(atom.arg(size/2).to_string().c_str());
        or_0 = !source_bool && (k_i_int == 0) && (source_int == dest_int);
        for (int j=1; j<size/2;j++) {
                if (atom.arg(j).get_sort().sort_kind() == Z3_UNINTERPRETED_SORT) {
                        z3::expr arg_j_int = z3_ctx.int_const(atom.arg(j).to_string().c_str());
                        z3::expr arg_j2_int = z3_ctx.int_const(atom.arg(j+size/2).to_string().c_str());
                        or_0 = or_0 && (arg_j_int == arg_j2_int);
                } else {
                        or_0 = or_0 && (atom.arg(j)==atom.arg(j+size/2));
                }
        }
        return or_0;
}


z3::expr listsolver::pred_unfold_ge1(z3::expr& atom, int i, z3::expr_vector& new_bools) {
    std::string source = atom.arg(0).to_string();
    std::string new_name = "["+source+","+to_string(i)+"]";
//    std::string pred_name = atom.decl().name().str();
//    int index = index_of_pred(pred_name);
//    predicate pred = m_ctx.get_pred(index); // get predicate definition
	Predicate_SLID_INT *pred = dynamic_cast<Predicate_SLID_INT*>(m_problem->getPredicate());
    int size = atom.num_args() - pred->size_of_static_parameters(); // size of source and destination paramaters
    z3::expr source_bool = z3_ctx.bool_const(new_name.c_str()); // [Z1,i]
    new_bools.push_back(source_bool);
    z3::expr source_int = z3_ctx.int_const(source.c_str()); // Z1

    // 1.2 predicate atom
    // 1.2.1 supposing atom is empty
    std::string k_name = "[k,"+to_string(i)+"]";
    z3::expr k_i_int = z3_ctx.int_const(k_name.c_str()); // k_i
    // 1.2.2 supposing atom is not emtpy
    z3::expr phi_pd = delta_ge1_predicate; // the predicate data closure
    z3::expr_vector args = pred->get_pars();
    z3::expr_vector f_args(z3_ctx); // predicate parameters, formal parameters
    z3::expr_vector a_args(z3_ctx); // actual parameters

    // init formla parameters and actual parameters
    for (int i=0; i<atom.num_args(); i++) {
            if (atom.arg(i).get_sort().sort_kind() != Z3_UNINTERPRETED_SORT) {
                    f_args.push_back(args[i]);
                    a_args.push_back(atom.arg(i));
            }
    }
    z3::expr k_int = z3_ctx.int_const("k"); // k is in data closure
    f_args.push_back(k_int);

    a_args.push_back(k_i_int);

    // logger() <<"formal pars: " << f_args << std::endl;
    // logger() <<"actual pars: " << a_args << std::endl;

    z3::expr or_1(z3_ctx); // by ufld_1
    z3::expr or_2(z3_ctx); // by ufld_ge_2

    int idx = pred->idx_E_gamma(); // check whether E ouccus in gamma
    // logger() << "idx: " << idx << std::endl;

    if (idx != -1) {
            // E occurs in gamma TOCHECK
            z3::expr E = atom.arg(0);
            z3::expr beta_idx = atom.arg(size/2+idx+1);
            z3::expr beta_idx_int = z3_ctx.int_const(beta_idx.to_string().c_str());

            std::string beta_idx_name = "["+beta_idx.to_string()+","+to_string(i)+"]";
            z3::expr beta_idx_bool = z3_ctx.bool_const(beta_idx_name.c_str());
            new_bools.push_back(beta_idx_bool); // new bool var

            // ufld_1
            z3::expr ufld_1 = (source_int == beta_idx_int && k_int == 1 && phi_pd);
            // logger() << "ufld_1: " << ufld_1 << std::endl;
            or_1 = ((source_bool && source_int>0 && beta_idx_bool && beta_idx_int>0) && ufld_1.substitute(f_args, a_args));
            // logger() << "or_1: " << or_1 << std::endl;
            // ufld_ge_2
            z3::expr ufld_ge_2 = (source_int != beta_idx_int && k_int >= 2 && phi_pd);
            or_2 = ((source_bool && source_int>0 && beta_idx_bool && beta_idx_int>0) && ufld_ge_2.substitute(f_args, a_args));
            // logger() << "or_2: " << or_2 << std::endl;
    } else {
            // E does not occur in gamma
            // ufld_1
            z3::expr ufld_1 = (k_int == 1 && phi_pd);
            // std::cout << "ufld_1: " << ufld_1 << std::endl;
            or_1 =  source_bool && source_int>0 && ufld_1.substitute(f_args, a_args);
            // logger() << "or_1: " << or_1 << std::endl;
            // ufld_ge_2
            z3::expr ufld_ge_2 = (k_int >= 2 && phi_pd);
            or_2 = source_bool && source_int>0 && ufld_ge_2.substitute(f_args, a_args);
            // logger() << "or_2: " << or_2 << std::endl;
    }
    return or_1 || or_2;
}

/**
 * atom in m -> dot string
 * @param m: the model of abstraction
 * @param atom : the space atom in sigam
 * @param i : the index
 * @param n : the size of sigam
 * @return str in dot
 */
//std::string listsolver::get_model_of_atom(z3::model &m, z3::expr &atom, int i, int n) {
//        std::string s = "";
//        if (atom.decl().name().str() == "pto") {
//                // 1.1 pto atom
//                std::string node_str="";
//                std::string edge_str="";
//
//                write_pto(m, atom, i, n, node_str, edge_str);
//
//                // std::cout << "node_str: " << node_str << std::endl;
//                // std::cout << "edge_str: \n" << edge_str << std::endl;
//
//                s = node_str + "\n" + edge_str + "\n";
//
//        } else {
//                //1.2 predicate atom
//                // 1.2.1 get k
//                std::string node_str="";
//                std::string edge_str="";
//                std::string k_name = m_ctx.logger().string_format("[k,%d]", i);
//                z3::expr k_i_int = z3_ctx().int_const(k_name.c_str()); // k_i
//                z3::expr k_i_interp = get_interp(m, k_i_int);
//                // std::cout << "model: " << m << std::endl;
//                int k = 0;
//                if (k_i_interp.to_string() != "nil") {
//                        k = k_i_interp.get_numeral_int();
//                }
//                // std::cout << "k: " << k << std::endl;
//                if (k > 0) {
//                        // 1.2.2 get predicate
//                        std::string pred_name = atom.decl().name().str();
//                        int index = index_of_pred(pred_name);
//                        predicate pred = m_ctx.get_pred(index); // get predicate definition
//                        int size = atom.num_args() - pred.size_of_static_parameters(); // size of source and destination paramaters
//                        pred_rule rule = pred.get_rule(0);
//                        z3::expr data = rule.get_data();
//                        z3::expr pto = rule.get_pto();
//                        z3::expr rec_app = rule.get_rec_apps()[0];
//                        z3::expr plfld = pred.get_plfld();
//
//                        // std::cout << "plfld: " << pred.get_plfld() << std::endl;
//
//                        z3::expr_vector f_args = pred.get_pars(); // predicate parameters, formal parameters
//                        z3::expr_vector a_args(z3_ctx()); // actual parameters
//
//                        // init formla parameters and actual parameters
//                        for (int j=0; j<atom.num_args(); j++) {
//                                a_args.push_back(atom.arg(j));
//                        }
//
//                        z3::expr_vector x_h(z3_ctx());
//                        rule.get_x_h(x_h);
//                        // z3::expr_vector x_h_cons(z3_ctx());
//                        for (int j=0; j<x_h.size(); j++) {
//                                f_args.push_back(x_h[j]);
//                                z3::sort st = x_h[j].get_sort();
//                                std::string name = x_h[j].to_string();
//                                name = name.replace(name.find(":"), 1, "");
//                                name = name.replace(name.find(" "), 1, "_");
//                                std::string ss = logger().string_format("%s_%s_%d_%d",   name.substr(1, name.length()-2).c_str(), pred_name.c_str(), i, 1);
//                                a_args.push_back(z3_ctx().constant(ss.c_str(), st));
//                        }
//
//                        // std::cout << "f_args: " << f_args << std::endl;
//                        // std::cout << "a_args: " << a_args << std::endl;
//
//                        //
//                        int idx = pred.idx_E_gamma(); // check whether E ouccus in gamma
//                        logger() << "idx: " << idx << std::endl;
//
//                        int node_i = 1;
//                        pto = pto.substitute(f_args, a_args);
//
//                        std::string node_name = "";
//
//                        z3::solver sol(z3_ctx());
//                        sol.add(z3_ctx().bool_val(true));
//                        sol.check();
//                        z3::model data_m = sol.get_model();
//                        z3::expr data_eval = data;
//
//
//                        while(node_i <= k) {
//                                // node_1
//                                z3::expr plfld_interp(z3_ctx());
//
//                                if (node_i == k) {
//                                        plfld_interp = get_interp(m, a_args[size/2]);
//                                } else if(node_i==k-1 && idx != -1){
//                                        plfld_interp = get_interp(m, a_args[size/2+1]);
//                                }
//
//                                if (node_i == 1) {
//                                        // E ->
//                                        write_pred_pto(m, data_m, pto, i, n, plfld_interp, plfld, node_i, k, node_str, edge_str);
//
//                                        // std::cout << "node_str: " << node_str << std::endl;
//                                        // std::cout << "edge_str: " << edge_str << std::endl;
//                                        s += node_str + "\n" + edge_str + "\n";
//
//                                } else {
//                                        node_str = "";
//                                        edge_str = "";
//
//                                        // recompute f_args and a_args
//                                        f_args.resize(0);
//                                        for (int j=0; j<a_args.size(); j++) {
//                                                f_args.push_back(a_args[j]);
//                                        }
//                                        a_args.resize(0);
//                                        for (int j=0; j<rec_app.num_args(); j++) {
//                                                a_args.push_back(rec_app.arg(j));
//                                        }
//
//                                        for (int j=0; j<x_h.size(); j++) {
//                                                z3::sort st = x_h[j].get_sort();
//                                                std::string name = x_h[j].to_string();
//                                                name = name.replace(name.find(":"), 1, "");
//                                                name = name.replace(name.find(" "), 1, "_");
//                                                std::string ss =  logger().string_format("%s_%s_%d_%d",  name.substr(1, name.length()-2).c_str(), pred_name.c_str(), i, node_i);
//                                                a_args.push_back(z3_ctx().constant(ss.c_str(), st));
//                                        }
//
//                                        // std::cout << "f_args: " << f_args << std::endl;
//                                        // std::cout << "a_args: " << a_args << std::endl;
//
//                                        pto = pto.substitute(f_args, a_args);
//
//                                        sol.reset();
//                                        sol.add(data_eval);
//                                        sol.check();
//                                        data_m = sol.get_model();
//
//                                        if (node_i == k && idx!=-1) {
//                                                z3::expr_vector args1(z3_ctx());
//                                                args1.push_back(pto.arg(0));
//                                                z3::expr_vector args2(z3_ctx());
//                                                args2.push_back(a_args[size/2+1]);
//                                                pto = pto.substitute(args1, args2);
//                                        }
//
//                                        // std::cout << "pto: " << pto << std::endl;
//                                        // std::cout << "data: " << data << std::endl;
//                                        // std::cout << "rec_app: " << rec_app << std::endl;
//                                        write_pred_pto(m, data_m, pto, i, n, plfld_interp, plfld, node_i, k, node_str, edge_str);
//
//                                        // std::cout << "node_str: " << node_str << std::endl;
//                                        // std::cout << "edge_str: " << edge_str << std::endl;
//                                        s += node_str + "\n" + edge_str + "\n";
//                                }
//                                data = data.substitute(f_args, a_args);
//                                data_eval = m.eval(data);
//                                rec_app = rec_app.substitute(f_args, a_args);
//
//                                node_i ++;
//                        }
//                }
//        }
//        return s;
//}

/**
 * check whether the source is allocated
 * @param m : the model of abstraction
 * @params source : the source
 * @params n : the atom size
 * @return true or false
 */
//bool listsolver::is_allocated(z3::model &m, z3::expr &source, int n) {
//        if (source.get_sort().sort_kind() == Z3_UNINTERPRETED_SORT) {
//                if (source.to_string().find("var_") == 0) return true;
//
//                for (int i=0; i<n; i++) {
//                        std::string source_name = "["+source.to_string()+","+to_string(i)+"]";
//                        z3::expr source_bool = z3_ctx().bool_const(source_name.c_str());
//                        if (m.has_interp(source_bool.decl())) {
//                                z3::expr bool_interp = m.get_const_interp(source_bool.decl());
//                                return bool_interp.to_string() == "true";
//                        }
//                }
//        }
//        return false;
//}

/**
 * translate pto into dot_str
 * @param m : the model of abstraction
 * @param pto : the pto atom
 * @param n : the atom num
 * @param node_str the output
 * @param edge_str the output
 */
//void listsolver::write_pto(z3::model& m, z3::expr& pto, int i, int n, std::string& node_str, std::string& edge_str) {
//        z3::expr source = pto.arg(0);
//        z3::expr source_interp = get_interp(m, source);
//        std::string node_name = logger().string_format("\"node_%s\"", source_interp.to_string().c_str());
//        node_str = logger().string_format("%s [label=\"atom_%d|<f0>(%s)", node_name.c_str(), i, source_interp.to_string().c_str());
//
//        z3::expr sref = pto.arg(1);
//        if (sref.decl().name().str() == "sref") {
//                for (int j=0; j<sref.num_args(); j++) {
//                        z3::expr ref = sref.arg(j);
//                        std::string flag_str = "";
//                        z3::expr dest = ref.arg(1);
//                        z3::expr dest_interp = get_interp(m, dest);
//
//
//                        flag_str = logger().string_format("<f%d>%s", (j+1), ref.arg(0).to_string().c_str());
//
//                        if (is_allocated(m, dest, n)) {
//                                edge_str = logger().string_format("%s%s:f%d->\"node_%s\":f0;\n", edge_str.c_str(), node_name.c_str(), (j+1), dest_interp.to_string().c_str());
//                        }
//
//                        node_str +=  logger().string_format("|%s:(%s)",  flag_str.c_str(),  dest_interp.to_string().c_str());
//                }
//        } else {
//                std::string flag_str = "";
//                z3::expr dest = sref.arg(1);
//                z3::expr dest_interp = get_interp(m, dest);
//                flag_str = logger().string_format("<f%d>%s", 1, sref.arg(0).to_string().c_str());
//
//                if (is_allocated(m, dest, n)) {
//                        edge_str = logger().string_format("%s%s:f%d->\"node_%s\":f0;\n", edge_str.c_str(), node_name.c_str(), 1, dest_interp.to_string().c_str());
//                }
//
//                node_str +=  logger().string_format("|%s:(%s)",  flag_str.c_str(),  dest_interp.to_string().c_str());
//        }
//        node_str += "\"];\n";
//}

/**
 * translate pred_pto into dot_str
 * @param m : the model of abstraction
 * @param pto : the pto atom
 * @param n : the atom num
 * @param plfld_interp : plfld_interp (the last two)
 * @param plfld : plfld
 * @param node_i : node_i times
 * @param k : the predicate times
 * @param node_str the output
 * @param edge_str the output
 */
//void listsolver::write_pred_pto(z3::model& m, z3::model& data_m, z3::expr& pto, int i, int n, z3::expr& plfld_interp, z3::expr& plfld, int node_i, int k, std::string& node_str, std::string& edge_str) {
//        z3::expr source = pto.arg(0);
//        z3::expr source_interp = get_interp(m, source);
//        std::string node_name = logger().string_format("\"node_%s\"",  source_interp.to_string().c_str());
//        node_str = logger().string_format("%s [label=\"atom_%d|<f0>(%s)", node_name.c_str(), i,  source_interp.to_string().c_str());
//        z3::expr sref = pto.arg(1);
//        for (int j=0; j<sref.num_args(); j++) {
//                z3::expr ref = sref.arg(j);
//                std::string flag_str = "";
//                z3::expr dest = ref.arg(1);
//                z3::expr dest_interp = get_interp(m, dest);
//
//                if (dest.get_sort().sort_kind() == Z3_UNINTERPRETED_SORT) {
//                        if (plfld.to_string() == ref.arg(0).to_string() && Z3_ast(plfld_interp)!=0) {
//                                dest_interp = plfld_interp;
//                        }
//                } else {
//                        dest_interp = get_interp(data_m, dest);
//                }
//
//                flag_str = logger().string_format("<f%d>%s", (j+1), ref.arg(0).to_string().c_str());
//
//                if (is_allocated(m, dest, n) && dest_interp.to_string()!="nil") {
//                        edge_str = logger().string_format("%s%s:f%d->\"node_%s\":f0;\n", edge_str.c_str(), node_name.c_str(), (j+1), dest_interp.to_string().c_str());
//                }
//
//                node_str +=  logger().string_format("|%s:(%s)",  flag_str.c_str(),  dest_interp.to_string().c_str());
//        }
//        node_str += "\"];\n";
//}


void listsolver::init_int_vector(std::vector<int> &vec, int size) {
        vec.clear();
        for (int i=0; i<size; i++) {
                vec.push_back(-1);
        }
}

int listsolver::index_of_int(int x, std::vector<int> &vec) {
        for (int i=0; i<vec.size(); i++) {
                if (vec[i] == x) return i;
        }
        return -1;
}
