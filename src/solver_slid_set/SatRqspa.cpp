#include "solver_slid_set/SatRqspa.h"



//extern z3::context z3_ctx;
//extern Z3Buffer z3_buffer;

/**
 * get line content :
 * @param str : format "  : content "
 */
void SatRqspa::getContent(std::string& str) {
    int start = str.find(":");
    str = str.substr(start+2, str.size());
    boost::trim(str);
}

/**
 * get transition
 * @param str : format : "State $src :0XXX -> State $dst"
 */
void SatRqspa::getTransition(std::string& str, transition& tr) {
    int start = str.find(" ");
    int end = str.find(":");
    std::string src = str.substr(start+1, end-start-1);
    tr.src = boost::lexical_cast<int>(src);

    start = end;
    end = str.find("->");

    std::string info = str.substr(start+2, end-start-3);
    for (int i=0; i<info.size(); i++) {
        tr.info.push_back(info.substr(i, 1));
    }

    start = str.find("state", end);
    end = str.size()-1;

    std::string dst = str.substr(start+6, end-start-5);
    tr.dst = boost::lexical_cast<int>(dst);
}

/**
 * read file, create fa
 * @param fa
 * @param file_name
 * @param prefix : the state prefix
 */
void SatRqspa::readFile(FA& fa, std::string file_name, std::string prefix) {
    std::ifstream fin(file_name);

    std::string str;
    getline(fin, str);// first blank line
    // read vars
    std::vector<std::string> vars;
    getline(fin, str);
    getContent(str);
    boost::split(vars, str, boost::is_any_of(" "));

    fa.setAlphabetSet(vars);

    // read init state
    getline(fin, str);
    getContent(str);
    int i_state = boost::lexical_cast<int>(str);
    fa.setInitState(i_state);

    // read accept states
    getline(fin, str);
    getContent(str);
    std::vector<std::string> fstr_states;
    boost::split(fstr_states, str, boost::is_any_of(" "));
    std::vector<int> f_states;
    for (int i=0; i<fstr_states.size(); i++) {
        f_states.push_back(boost::lexical_cast<int>(fstr_states[i]));
    }
    fa.setAcceptStates(f_states);

    // skip 2 lines
    getline(fin, str); getline(fin, str);

    // get number of states
    getline(fin, str);
    int start = str.find("has");
    int end = str.find("states");
    str = str.substr(start+3, end-start-3);
    boost::trim(str);
    int number = boost::lexical_cast<int>(str);

    fa.addStates(number, prefix);

    // read transitions
    getline(fin, str);

    // int t_count = 0;
    while(getline(fin, str)) {
        if (str.find("State") != 0) break;
        transition tr;
        getTransition(str, tr);
        fa.addTransition(tr);
        // std::cout << "transition: " << tr.src << "-> " <<  tr.dst << " : " << tr.info[0] << std::endl;
        // t_count ++;
    }
    // std::cout << "tr count: " << t_count << std::endl;

    fin.close();
}

/**
 * var in phi_count -> NFA
 * @param : var
 * @param : nfa [with alphabet]
 */
void SatRqspa::generateNFA(z3::expr var, FA& nfa) {
    int total = nfa.getAlphabet().size();
    int idx = nfa.gePos(var.to_string());
    std::string prefix = var.to_string();
    prefix.append("_");

    transition tr;
    for (int i=0; i<total; i++) {
        tr.info.push_back("X");
    }

    std::vector<int> accept_states;

    if (var.get_sort().to_string() == "SetInt") {
        nfa.addStates(4, prefix);
        accept_states.push_back(3);

        // add transitions
        // 0->1 : X
        tr.src = 0; tr.dst = 1; tr.info[idx] = "X";
        nfa.addTransition(tr);

        tr.src = 1; tr.dst = 1; tr.info[idx] = "0";
        nfa.addTransition(tr); // 1->1
        tr.dst = 2; tr.info[idx] = "1";
        nfa.addTransition(tr); // 1->2
        tr.dst = 3;
        nfa.addTransition(tr); // 1->3
        tr.src = 2;
        nfa.addTransition(tr); // 2->3
        tr.dst = 2; tr.info[idx] = "X";
        nfa.addTransition(tr); // 2->2
        tr.src = 3; tr.dst=3; tr.info[idx] = "0";
        nfa.addTransition(tr); // 3->3

    } else {
        nfa.addStates(3, prefix);
        accept_states.push_back(2);
        // add transitions
        tr.src = 0; tr.dst = 1; tr.info[idx] = "X";
        nfa.addTransition(tr);

        tr.src = 1; tr.dst = 1; tr.info[idx] = "0";
        nfa.addTransition(tr); // 1->1
        tr.dst = 2; tr.info[idx] = "1";
        nfa.addTransition(tr); // 1->2
        tr.src = 2; tr.info[idx] = "0";
        nfa.addTransition(tr); // 2->2
    }

    nfa.setAcceptStates(accept_states);
}

/**
 * gnerate first order var relation with new vars
 * @param idx : index in vars of phi_count
 */
z3::expr SatRqspa::generateFovarExpr(int idx) {

    z3::expr_vector sum_items(z3_ctx);
    for (int cur : m_new_ids) {
        // i-th
        int temp = cur % m_factors[idx];
        temp = temp / m_factors[idx+1];
        if (temp == 0) {
            std::string x_name = "x_";
            x_name.append(std::to_string(cur));
            sum_items.push_back(z3_ctx.int_const(x_name.c_str()));
        }
    }
    return m_vars[idx] == (z3::sum(sum_items)-1);
}

/**
 * gnerate second order var relation with new vars
 * @param idx : index in vars of phi_count
 */
z3::expr SatRqspa::generateSovarExpr(int idx) {
    z3::expr result = z3_ctx.bool_val(true);

    z3::expr_vector sum0_items(z3_ctx);
    z3::expr_vector sum1_items(z3_ctx);
    sum1_items.push_back(z3_ctx.int_val(0));
    for (int cur : m_new_ids) {
        // i-th
        int temp = cur % m_factors[idx];
        temp = temp / m_factors[idx+1];
        std::string x_name = "x_";
        x_name.append(std::to_string(cur));
        if (temp == 1) {
            sum0_items.push_back(z3_ctx.int_const(x_name.c_str()));
        } else if (temp == 2) {
            sum1_items.push_back(z3_ctx.int_const(x_name.c_str()));
        }
    }

    std::string var_name = "min_";
    var_name.append(m_vars[idx].to_string());
    z3::expr min_var = z3_ctx.int_const(var_name.c_str());
    var_name = "max_";
    var_name.append(m_vars[idx].to_string());
    z3::expr max_var = z3_ctx.int_const(var_name.c_str());

    z3::expr sigma0 = z3::sum(sum0_items);
    // std::cout << "sigma0: " << sigma0 << std::endl;
    // exit(0);
    result = (min_var == sigma0 - 1) && (max_var == min_var + z3::sum(sum1_items));

    return result;
}

/**
 * get vars in phi_count -> m_vars
 *
 */
void SatRqspa::getVars() {
    Z3ExprSet fovs;
    Z3ExprSet sovs;
    z3_buffer.getFoVars(m_phi_count, fovs);
    z3_buffer.getSoVars(m_phi_count, sovs);
    m_vars.insert(m_vars.end(), fovs.begin(), fovs.end());
    m_vars.insert(m_vars.end(), sovs.begin(), sovs.end());
}


FA SatRqspa::generatePA() {

    FA phi_core(z3_ctx, z3_buffer);
    readFile(phi_core, m_file_name, "q_");

    std::cout << "accept: " << phi_core.getAcceptStates().size() << ", states: " << phi_core.getStateNum() << std::endl;

    phi_core = phi_core.getFlow(); // get nfa with one accept state

    phi_core.print("phi_core.dot");


    // state_code
    std::vector<int> state_code;
    int total = phi_core.getStateNum();
    state_code.push_back(total);
    //
    getVars();
    // std::cout << "get vars : " << m_vars.size() << std::endl;
    // generate nfa for var in phi_count
    std::vector<FA> nfas;
    for (int i=0; i<m_vars.size(); i++) {
        FA t_fa(z3_ctx, z3_buffer);
        t_fa.setAlphabetSet(phi_core.getAlphabet());
        generateNFA(m_vars[i], t_fa);
        nfas.push_back(t_fa);
        // t_fa.print("t_fa.dot");
        state_code.push_back(t_fa.getStateNum());
    }

    // compute factors by state_code
    int acc = 1;
    // std::vector<int> factors;
    m_factors.push_back(1);

    for (int i=state_code.size()-1; i>0; i--) {
        acc *= state_code[i];
        m_factors.insert(m_factors.begin(), acc);
    }

    // compute product of all nfa
    FA fa_result(z3_ctx,z3_buffer);
	fa_result = phi_core;
    for (int i=0; i<nfas.size(); i++) {
        // std::string fa_name = "fa_result_produce_";
        // fa_name.append(std::to_string(i)).append(".dot");
        fa_result = fa_result.product(nfas[i]);
        // fa_result.print(fa_name);
    }
    m_result = fa_result;
    fa_result.print("fa_result.dot");
    FA pa(z3_ctx,z3_buffer);
	pa = fa_result.stateAsEdge();

    pa.print("pa_before.dot");

    return pa;

}

/**
 * pa -> z3 expr
 *
 */
z3::expr SatRqspa::generateExpr(FA& pa) {
    // henuristic

    // pa = pa.getSubgraph();

    // pa.print("pa.dot");


    // std::set<int> x_ids;
    int accept_state = pa.getAcceptStates()[0];

    // std::cout << "compute flow : 0 -> " << accept_state << std::endl;

    // pa.printFlow(accept_state);
    m_new_ids.clear();
    z3::expr pa_phi = pa.toExpr(z3_ctx, accept_state, m_new_ids, m_tpaq_set);


    // generate expr relation with vars in phi_count and new vars

    z3::expr_vector var_items(z3_ctx);

    for (int i=0; i<m_vars.size(); i++) {
        if (m_vars[i].get_sort().to_string() == "SetInt") {
            var_items.push_back(generateSovarExpr(i));
        } else {
            var_items.push_back(generateFovarExpr(i));
        }
    }

    z3::expr var_item = z3::mk_and(var_items);

    z3::expr flow_item = pa_phi && var_item;

    // std::cout << "phi_count: " << phi_count << std::endl;

    z3::expr_vector ge_zero_items(z3_ctx);

    // std::cout << "tpaq size: " << m_tpaq_set.size() << std::endl;

    for (z3::expr tpaq : m_tpaq_set) {
        ge_zero_items.push_back(tpaq >= 0);
    }
    z3::expr ge_zero = z3::mk_and(ge_zero_items);

    // z3::expr result = flow_items[1]  && phi_count; //  phi_count &&
    z3::expr result = flow_item && ge_zero; //  && phi_count;


    return result;
}

z3::expr SatRqspa::subPhiCount() {
    // substitute phi_count
    z3::expr_vector src(z3_ctx);
    z3::expr_vector dst(z3_ctx);
    for (int i=0; i<m_vars.size(); i++) {
        if (m_vars[i].get_sort().to_string() == "SetInt") {
            z3::expr min_var = z3_buffer.getMin(m_vars[i]);
            z3::expr max_var = z3_buffer.getMax(m_vars[i]);
            src.push_back(min_var);
            src.push_back(max_var);
            std::string var_name = "min_";
            var_name.append(m_vars[i].to_string());
            z3::expr min_v = z3_ctx.int_const(var_name.c_str());
            var_name = "max_";
            var_name.append(m_vars[i].to_string());
            z3::expr max_v = z3_ctx.int_const(var_name.c_str());
            dst.push_back(min_v);
            dst.push_back(max_v);
        }
    }

    z3::expr phi_count = m_phi_count.substitute(src, dst);
    return phi_count;
}


/**
 * check sat the phi of pa
 * @param vars : vars in model
 * @param m_model : m_model
 * @return check_result
 */
z3::check_result SatRqspa::checkSat(std::vector<z3::expr>& vars, std::map<std::string, std::string>& m_model) {
    FA pa = generatePA();
    z3::expr phi_count = subPhiCount();

    int N = 1;
    FA sub_pa(z3_ctx, z3_buffer);
    for (int i=1; i<=N+1; i++) {
        if (i <= N) {
            sub_pa = pa.getSubgraph(i);
            sub_pa.print("pa_henuristic_"+std::to_string(i)+".dot");
        } else {
            sub_pa = pa;
        }

        z3::expr pa_phi = generateExpr(sub_pa) && phi_count;

        z3::solver solver(z3_ctx);
        solver.add(pa_phi);
        z3::check_result result = solver.check();

        if (result == z3::sat) {

            // get model
            z3::model model = solver.get_model();

            // displayModel(model, vars, m_model);

            return z3::sat;
        }
    }

    return z3::unsat;
}


void SatRqspa::displayModel(z3::model& model, std::vector<z3::expr>& vars, std::map<std::string, std::string>& m_model) {
    // std::cout << "tpaq size: " << m_tpaq_set.size() << std::endl;
    std::map<std::string, int> edge_to_count; // edge -> times
    for (z3::expr tpaq : m_tpaq_set) {
        if (model.has_interp(tpaq.decl())) {
            z3::expr val = model.get_const_interp(tpaq.decl());
            if (val.get_numeral_int() < 0) {
                std::cout << tpaq << "  LESS THAN 0!\n";
                exit(-1);
            }
            edge_to_count[tpaq.to_string()] = val.get_numeral_int();

        } else {
            std::cout<< tpaq << "   NO TPAQ.\n";
            exit(-1);
        }
    }


    std::vector< std::vector< std::string> > word;
    m_result.printModel("sat_model.dot", m_new_ids, edge_to_count, model, z3_ctx, word);

    /*
      for (int i=0; i<word.size(); i++) {
      std::cout << m_result.vecToStr(word[i], ", ") << std::endl;
      }
      std::cout << std::endl;
    */

    // std::cout << "word len: " <<  word.size() << std::endl;
    for (int i=0; i<vars.size(); i++) {
        z3::expr var = vars[i];
        std::string key = var.to_string();
        int pos = m_result.gePos(key);

        // std::cout << "var: "<< var << ", pos: "<< pos << ", sort: " << var.get_sort() <<std::endl;

        if (var.is_bool()) {
            // std::cout << "word[0]: " <<  m_result.vecToStr(word[0], ", ") << std::endl;
            // std::cout << "word[0][pos]: " << word[0][pos] << std::endl;
            if (word[0][pos] == "0") {
                m_model[key] = "false";
            } else {
                m_model[key] = "true";
            }
            // std::cout << key << ": " << m_model[key] << std::endl;
        } else {
            std::string val;
            std::vector<std::string> val_vec;
            for (int j=1; j<word.size(); j++) {
                if (word[j][pos] == "1") {
                    val = std::to_string(j-1);
                    if (var.is_int()) {
                        break;
                    } else {
                        val_vec.push_back(val);
                    }
                }
            }

            //
            if (val_vec.size() == 0) {
                m_model[key] = val;
            } else {
                std::string val_str = "{";
                val_str.append(m_result.vecToStr(val_vec, ", ")).append("}");
                m_model[key] = val_str;
            }
        }


        // std::cout << key << "->"  << m_model[key] << std::endl;

    }


    for (int i=0; i<m_vars.size(); i++) {
        z3::expr sov = m_vars[i];
        std::string var_name = "min_";
        var_name.append(sov.to_string());
        z3::expr min_v = z3_ctx.int_const(var_name.c_str());
        var_name = "max_";
        var_name.append(sov.to_string());
        z3::expr max_v = z3_ctx.int_const(var_name.c_str());
        z3::expr min_val = model.get_const_interp(min_v.decl());
        z3::expr max_val = model.get_const_interp(max_v.decl());


        std::cout << min_v << ": " << min_val << std::endl;
        std::cout << max_v << ": " << max_val << std::endl;
    }



    /*
    // x_i
    for (int id : m_new_ids) {
    std::string pre = "x_";
    pre.append(std::to_string(id));
    z3::expr x_i = z3_ctx.int_const(pre.c_str());
    if (model.has_interp(x_i.decl())) {
    z3::expr val = model.get_const_interp(x_i.decl());
    if (val.get_numeral_int() > 0) {
    std::cout << x_i << ": " << val << std::endl;
    } else {
    if (val.get_numeral_int() < 0) {
    std::cout << x_i << " LESS THAN 0!\n";
    exit(-1);
    }
    }
    }
    }
    */
}
