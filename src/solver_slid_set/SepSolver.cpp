/*******************************************
*  @file  SepSolver.cpp                    *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2019-2-18                     *
*                                          *
*******************************************/

#include "solver_slid_set/SepSolver.h"
//#include "component/Z3Buffer.h"
#include "solver_slid_set/MonaExecutor.h"
#include "solver_slid_set/MonaTranslator.h"
#include "solver_slid_set/SatRqspa.h"
#include "Types.h"

//extern z3::context z3_ctx;
//extern Z3Buffer z3_buffer;

SepSolver::SepSolver(z3::context& ctx, Z3Buffer& buffer)
: z3_ctx(ctx), z3_buffer(buffer), m_problem(nullptr),m_abs_phi(z3_ctx), m_phi_free_items(z3_ctx), m_abs_psi(z3_ctx), m_psi_free_items(z3_ctx), 
    m_new_vars(z3_ctx), m_counter(0) { }

SepSolver::~SepSolver() {
}

void SepSolver::setProblem(Problem* problem) {
    m_problem = problem;
    //m_problem->setSolver(this);
    expr m_phi = m_problem->getPhi();
    expr m_psi = m_problem->getPsi();
    initInfo(m_phi, m_phi_location, m_phi_location_relation);
    if(Z3_ast(m_psi) != nullptr){
    	initInfo(m_psi, m_psi_location, m_psi_location_relation);
	}
}


void SepSolver::solve() {
    string result = "";
    if (m_problem->isSat()) {
        result = checkSat();
    } else {
        result = checkEntl();
    }
    cout << "result: " << result <<endl;
}


string SepSolver::checkSat() {
    // cout << "check sat...\n";
    expr_vector free_items(z3_ctx);
    // m_problem->show();
    expr abs = getAbsPhi(free_items);
    // cout << abs <<endl;
    return check(abs, free_items);
}

string SepSolver::check(expr& abs, expr_vector& free_items) {
    if (free_items.size() == 0) {
        // simple case
//cout << "simple case: \n";
        MonaTranslator mona_tl(z3_ctx, z3_buffer, abs);
        mona_tl.writeToFile("test.mona");
        std::map<std::string, std::string> model;
        MonaExecutor mona_exe;
        mona_exe.setArgs("-q");
        mona_exe.setName("test.mona");
        // std::cout << "execute mona -q test.mona\n";
        bool is_sat = mona_exe.execute(model);
        // cout << "is_sat: " << is_sat << endl;
        if (is_sat) {
            return "SAT";
        }
        return "UNSAT";
    } else {
//cout << "complex case;\n";
        vector<expr> vars;
        Z3ExprSet bvars;
        Z3ExprSet fovars;
        Z3ExprSet sovars;
        z3_buffer.getBVars(abs, bvars);
        z3_buffer.getFoVars(abs, fovars);
        z3_buffer.getSoVars(abs, sovars);
        vars.insert(vars.end(), bvars.begin(), bvars.end());
        vars.insert(vars.end(), fovars.begin(), fovars.end());
        vars.insert(vars.end(), sovars.begin(), sovars.end());
        /*
        cout << "free_items:\n";
        for (int i=0; i<free_items.size(); i++) {
            cout << free_items[i] <<endl;
        }

        cout << "Var: ";
        for (auto e : vars) {
            cout << e << " ";
        }
        cout << endl;
        */

        int MAX_CTX = 1 << free_items.size();
        int ctx = MAX_CTX-1;

        while (ctx >= 0) {
            expr_vector dst(z3_ctx);
            expr_vector phi_count_items(z3_ctx);
            for (unsigned int i=0; i<free_items.size(); i++) {
                if (ctx & (1<<i)) {
                    // true
                    dst.push_back(z3_ctx.bool_val(true));
                    phi_count_items.push_back(free_items[i]); 
                } else {
                    // false
                    dst.push_back(z3_ctx.bool_val(false));
                    phi_count_items.push_back(!free_items[i]); 
                }
            }
            expr phi_core = abs.substitute(free_items, dst);

            expr phi_count = mk_and(phi_count_items);

            MonaTranslator mona_tl(z3_ctx, z3_buffer, phi_core);
            mona_tl.writeToFile("phi_core.mona");
            map<string, string> model;
            MonaExecutor mona_exe;
            mona_exe.setArgs("-q");
            mona_exe.setName("phi_core.mona");
            bool is_sat = mona_exe.execute(model);

            // cout << "ctx: " << ctx << " -- " << is_sat <<endl;
            if (is_sat) {
                mona_exe.setArgs("-w -u -q");
                mona_exe.setName("phi_core.mona");
                mona_exe.execute("phi_core.dfa");
                // construct PA
                SatRqspa rqspa(z3_ctx, z3_buffer, "phi_core.dfa", phi_count);
                z3::check_result sat_res = rqspa.checkSat(vars, model);
                if (sat_res == z3::sat) {
                    return "SAT";
                } else {
                    cout << "UNSAT\n";
                }
            }
            ctx ++;
        }
        return "UNSAT";
    }
}


string SepSolver::checkEntl() {
    // check phi and psi is sat
    // cout << "check entl....\n";
    // m_problem->show();
    expr_vector phi_free_items(z3_ctx);
    expr abs_phi = getAbsPhi(phi_free_items);
    string phi_res = check(abs_phi, phi_free_items);
    // cout << "abs_phi: " << abs_phi <<endl;
    // cout << "phi_res: " << phi_res <<endl;
    if (phi_res == "UNSAT") return "UNSAT";
    expr_vector psi_free_items(z3_ctx);
    expr abs_psi = getAbsPsi(psi_free_items);
    string psi_res = check(abs_psi, psi_free_items);
    if (psi_res == "UNSAT") return "SAT";
    // match
    initRm();
    Graph g_phi;
    Graph g_psi;
    constructPhiGraph(g_phi);
    constructPsiGraph(g_psi);

    printPhi(g_phi, "graph_phi.dot");

    // cout << "MATCHING....\n";
    bool res = checkAllocatingPlans(g_phi, g_psi);

    if (res) {
        return "UNSAT";
    } else {
        return "SAT";
    }
}

void SepSolver::initInfo(expr& phi, vector<expr>& locations, RelationMatrix& rm) {
    Z3ExprSet lvars;
    z3_buffer.getLVars(phi, lvars);
    locations.insert(locations.end(), lvars.begin(), lvars.end());
    for (int i=0; i<locations.size(); i++) {
        vector<int> v;
        for (int i=0; i<locations.size(); i++) {
            v.push_back(-1);
        }
        rm.push_back(v);
    }

    for (int i=0; i<phi.num_args()-1; i++) {
        if (phi.arg(i).num_args() > 0) {
            string s = phi.arg(i).arg(0).get_sort().to_string();
            if (s != "Bool" && s != "Int" && s != "SetInt") {
                expr E = phi.arg(i).arg(0);
                expr F = phi.arg(i).arg(1);
                int idx_E = z3_buffer.indexOf(locations, E);
                int idx_F = z3_buffer.indexOf(locations, F);
                string op = phi.arg(i).decl().name().str();
                if (op == "=") {
                    rm[idx_E][idx_F] = 1;
                } else {
                    rm[idx_E][idx_F] = 0;
                }
            }
        }
   }
}

void SepSolver::initRm() {
    initRm(m_abs_phi, m_phi_location, m_phi_location_relation, m_phi_free_items);
    // initPhiAllocated();
    initRm(m_abs_psi, m_psi_location, m_psi_location_relation, m_psi_free_items);
}

expr SepSolver::getAbsPhi(expr_vector& free_items) {
    if (Z3_ast(m_abs_phi) == nullptr) {
    	expr m_phi = m_problem->getPhi();
        m_abs_phi = getAbs(free_items, m_phi);
        for (int i=0; i<free_items.size(); i++) m_phi_free_items.push_back(free_items[i]);
    }
    return m_abs_phi;
}

expr SepSolver::getAbsPsi(expr_vector& free_items) {
    if (Z3_ast(m_abs_psi) == nullptr) {
    	expr m_psi = m_problem->getPsi();
        m_abs_psi = getAbs(free_items, m_psi);
        for (int i=0; i<free_items.size(); i++) m_psi_free_items.push_back(free_items[i]);
    }
    return m_abs_psi;
}



void SepSolver::initRm(expr& abs, vector<expr>& locations, RelationMatrix& rm, expr_vector& free_items) {
    for (int i=0; i<rm.size(); i++) {
        for (int j=i+1; j<rm.size(); j++) {
            if (rm[i][j] != 1) {
                expr E = z3_ctx.int_const(locations[i].to_string().c_str());
                expr F = z3_ctx.int_const(locations[j].to_string().c_str());
                expr check_f = abs && E!=F;
                if (check(check_f, free_items) == "UNSAT") {
                    rm[i][j] = 1;
                }
                // check_f = abs && E==F;
                // if (check(check_f, free_items) == "UNSAT") {
                //     rm[i][j] = 0;
                // } 
            }
        }
    }
}

void SepSolver::initPhiAllocated() {
	expr m_phi = m_problem->getPhi();
    expr space = m_phi.arg(m_phi.num_args()-1);
    for (int i=0; i<space.num_args(); i++) {
        m_phi_space_allocated.push_back(-1);
        expr atom = space.arg(i);
        if (atom.decl().name().str() != "pto") {
            string name = atom.arg(0).to_string();
            name = name.append("_BOOL_").append(to_string(i)); 
            expr new_bool = z3_ctx.bool_const(name.c_str());
            expr check_f = m_abs_phi && new_bool;
            string res = check(check_f, m_phi_free_items);
            if (res == "UNSAT") m_phi_space_allocated[i] = 0; 
        }
    }
}



expr SepSolver::getAbs(expr_vector& free_items, expr& phi) {

    expr_vector data_items(z3_ctx);
    data_items.push_back(z3_ctx.int_const("nil") == 0); // nil == 0
    int num = phi.num_args();
    for (int i=0; i<num-1; i++) {
        if (phi.arg(0).num_args() == 0) {
            if (phi.arg(0).to_string() == "true") data_items.push_back(z3_ctx.bool_val(true));
            else if (phi.arg(0).to_string() == "false") data_items.push_back(z3_ctx.bool_val(false));
        } else {
            string sort_name = phi.arg(i).arg(0).get_sort().to_string();
            if (sort_name == "Bool") {
                if (phi.arg(i).decl().name().str() == "implies") {
                    data_items.push_back(!(phi.arg(i).arg(0) && !phi.arg(i).arg(1)));
                } else {
                    data_items.push_back(phi.arg(i));
                }
                continue;
            }
            if (sort_name == "Int") {
                // check free items
                Z3ExprSet items;
                z3_buffer.getIntItems(phi.arg(i), items);
                if (items.size() > 2) {
                    free_items.push_back(phi.arg(i));
                }
            }
            // abstraction of data items
            if (sort_name == "SetInt" || sort_name == "Int") {
                data_items.push_back(phi.arg(i));
            } else {
                expr item1 = z3_ctx.int_const(phi.arg(i).arg(0).to_string().c_str());
                expr item2 = z3_ctx.int_const(phi.arg(i).arg(1).to_string().c_str());
                string fname = phi.arg(i).decl().name().str();
                if (fname == "=") {
                    data_items.push_back(item1 == item2);
                } else {
                    data_items.push_back(item1 != item2);
                }
            }
        }
    }
    expr data_abs = mk_and(data_items);
    // cout << "data_abs: " << data_abs <<endl;

    expr spatial = phi.arg(num-1);
    expr_vector spatial_abs_items(z3_ctx);
    expr_vector new_bools(z3_ctx);

    if (spatial.decl().name().str() == "sep") {
        num = spatial.num_args();
        for (int i=0; i<num; i++) {
            expr atom = spatial.arg(i);
            spatial_abs_items.push_back(getSpatialAbs(atom, i, new_bools, free_items));
        }
    } else {
        spatial_abs_items.push_back(getSpatialAbs(spatial, 0, new_bools, free_items));
    }
    
    expr spatial_abs = mk_and(spatial_abs_items);

    // cout << new_bools <<endl; 

    expr spatial_star = getSpatialStar(new_bools);

    expr abs = data_abs && spatial_abs && spatial_star;
    // expr abs = data_abs;

    // cout << "spatial_abs: " << spatial_abs <<endl;
    // cout << "spatial_star: " << spatial_star <<endl;

    // add var env
    for (unsigned int i=0; i<m_new_vars.size(); i++) {
        z3_buffer.addVarEnv(m_new_vars[i]);
    }
    for (unsigned int i=0; i<new_bools.size(); i++) {
        z3_buffer.addVarEnv(new_bools[i]);
    }

    return abs;
}


void SepSolver::showEqClass() {
    cout << "phi: \n";
    for (int i=0; i<m_phi_location_relation.size(); i++) {
        for (int j=0; j<m_phi_location_relation.size(); j++) {
            cout << m_phi_location[i] << ", " << m_phi_location[j] << " --> " << m_phi_location_relation[i][j] << "   ";
        }
        cout << endl;
    }
    cout << endl;
    
    for (int i=0; i<m_phi_location.size(); i++) {
        cout << m_phi_location[i] << " -- " << m_phi_location_eq_class[i] <<endl;
    }

    for (int i=0; i<m_phi_eq_class.size(); i++) {
        cout << "eq_" << i << ": [";
        for (auto idx : m_phi_eq_class[i]) cout << idx << ", ";
        cout << "]\n";

    }

    cout << "psi: \n";
    for (int i=0; i<m_psi_location.size(); i++) {
        cout << m_psi_location[i] << " -- " << m_psi_location_eq_class[i] <<endl;
    }

    for (int i=0; i<m_psi_eq_class.size(); i++) {
        cout << "eq_" << i << ": [";
        for (auto idx : m_psi_eq_class[i]) cout << idx << ", ";
        cout << "]\n";

    }
}


void SepSolver::showRM() {
    for (int i=0; i<m_phi_location_relation.size(); i++) {
        for (int j=i+1; j<m_phi_location_relation.size(); j++) {
            expr E = z3_ctx.int_const(m_phi_location[i].to_string().c_str());
            expr F = z3_ctx.int_const(m_phi_location[j].to_string().c_str());
            cout << E << " == " << F << " : " << m_phi_location_relation[i][j] <<endl;
        }
    }
}

expr SepSolver::getSpatialAbs(expr& atom, int i, expr_vector& new_bools, expr_vector& free_items) {
    ostringstream oss;

    string new_name;
    oss << atom.arg(0) << "_BOOL_" << i; 
    expr nbool = z3_ctx.bool_const(oss.str().c_str());
    new_bools.push_back(nbool);

    if (atom.decl().name().str() == "pto") {
        return nbool;
    } else {
        expr_vector bool_items(z3_ctx);
        bool_items.push_back(nbool);
        int num = atom.num_args();

        //
        expr_vector nil_items(z3_ctx);
        expr nil = z3_ctx.int_const("nil");
        expr E = z3_ctx.int_const(atom.arg(0).to_string().c_str());
        expr F = z3_ctx.int_const(atom.arg(num/2).to_string().c_str());
        expr S_E = atom.arg(num/2-1); 
        expr S_F = atom.arg(num-1); 
        nil_items.push_back(!(E == nil &&  !(S_E == z3_buffer.getEmptyset())));  
        nil_items.push_back(!(F == nil &&  !(S_F == z3_buffer.getEmptyset())));  

        expr nil_cond = mk_and(nil_items);
        

        // unfold 0
        expr_vector items(z3_ctx);
        
        for (int i=0; i<num/2; i++) {
            if (atom.arg(i).get_sort().to_string() == "SetInt") {
                items.push_back(atom.arg(i) == atom.arg(i+num/2));
            } else {
                items.push_back(z3_ctx.int_const(atom.arg(i).to_string().c_str()) ==
                    z3_ctx.int_const(atom.arg(i+num/2).to_string().c_str()));
            }
        }
        expr unfold0 = !nbool && mk_and(items);
        // cout << "unfold0: " << unfold0 <<endl;
   
        // unfold 1
        expr_vector src_pars(z3_ctx);
        expr_vector dst_pars(z3_ctx);
        Predicate_SLID_SET *m_pred = dynamic_cast<Predicate_SLID_SET*>(m_problem->getPredicate()); 
        expr_vector& pars = m_pred->getPars();
        int idx = m_pred->getEinGamma();
        if (idx != -1) {
            src_pars.push_back(z3_ctx.int_const(pars[0].to_string().c_str()));
            src_pars.push_back(z3_ctx.int_const(pars[pars.size()/2+1].to_string().c_str()));
            dst_pars.push_back(z3_ctx.int_const(atom.arg(0).to_string().c_str()));
            dst_pars.push_back(z3_ctx.int_const(atom.arg(pars.size()/2+1).to_string().c_str()));
            string name = atom.arg(pars.size()/2+idx+1).to_string() + "_BOOL_" + to_string(i);
            expr nbool_idx = z3_ctx.bool_const(name.c_str());
            bool_items.push_back(nbool_idx);
            new_bools.push_back(nbool_idx);
        }
        for (unsigned j=0; j<pars.size(); j++) {
            if (pars[j].get_sort().to_string() == "SetInt") {
                src_pars.push_back(pars[j]);
                dst_pars.push_back(atom.arg(j));
            }
        }
        // bool_prefix
        expr bool_prefix = mk_and(bool_items);
        // unfold1
        // cout << "src_pars:" << src_pars <<endl;
        // cout << "dst_pars:" << dst_pars <<endl;
        expr unfold1 = bool_prefix && m_pred->getUnfold1().substitute(src_pars, dst_pars);
        // cout << "unfold1: " << unfold1 <<endl;
        // unfold2
        expr unfold2 = bool_prefix && m_pred->getUnfold2(m_new_vars).substitute(src_pars, dst_pars);

        expr free_item = m_pred->getFreeItem();
        if (Z3_ast(free_item) != 0) {
            for (unsigned int j=0; j<free_item.num_args(); j++) {
                free_items.push_back(free_item.arg(j).substitute(src_pars, dst_pars));
            }
        }

        // cout << "unfold0: " << unfold0 <<endl;
        // cout << "unfold1: " << unfold1 <<endl;
        // cout << "unfold2: " << unfold2 <<endl;
        return nil_cond && !(!unfold0 && !unfold1 && !unfold2);
    }
}

expr SepSolver::getSpatialStar(expr_vector& new_bools) {
    expr_vector items(z3_ctx);
    items.push_back(z3_ctx.bool_val(true));

    for (unsigned int i=0; i<new_bools.size(); i++) {
        for (unsigned int j=i+1; j<new_bools.size(); j++) {
            string name1 = new_bools[i].to_string();
            string name2 = new_bools[j].to_string();
            int idx1 = getSuffixIdx(name1);
            int idx2 = getSuffixIdx(name2);
            if (idx1 != idx2) {
                items.push_back(!(z3_ctx.int_const(name1.c_str()) == z3_ctx.int_const(name2.c_str()) 
                    && new_bools[i] && new_bools[j]));
            }
        }
    }
    return mk_and(items);
}

int SepSolver::getSuffixIdx(string& str) {
    int i = str.find("_BOOL_");
    string istr = str.substr(i+6, str.length()-i-6);
    str = str.substr(0, i);
    return stoi(istr);
}


void SepSolver::_computeEqClass(RelationMatrix& rm, 
    vector<int>& loc_eq, vector<set<int>>& eq_class) {
    // clear
    loc_eq.clear();
    eq_class.clear();

    for (int i=0; i<rm.size(); i++) {
        loc_eq.push_back(-1);
    }
    set<int> eq;
    for (int i=0; i<rm.size(); i++) {
        for (int j=i+1; j<rm.size(); j++) {
            if (rm[i][j] == 1 || rm[j][i] == 1) {
                if (loc_eq[i] == -1) {
                    loc_eq[i] = eq_class.size();
                    loc_eq[j] = loc_eq[i];
                    eq.clear();
                    eq.insert(i);
                    eq.insert(j);
                    eq_class.push_back(eq);
                } else {
                    loc_eq[j] = loc_eq[i];
                    eq_class[loc_eq[i]].insert(j);
                }
            }
        }
    }

    for (int i=0; i<rm.size(); i++) {
        if (loc_eq[i] == -1) {
            loc_eq[i] = eq_class.size();
            eq.clear();
            eq.insert(i);
            eq_class.push_back(eq);
        }
    }
}


bool SepSolver::checkAllocatingPlans(Graph& g_phi, Graph& g_psi) {
	expr m_phi = m_problem->getPhi();
	
    vector<int> cc_cycle_num = g_phi.getCcCycleNum();
    int cc_num = cc_cycle_num.size();
    int cc_total = 0;
    // cout << "cc_cyles: ";
    for (int i=0; i<cc_num; i++) {
        // cout << cc_cycle_num[i] << " ";
        cc_total += cc_cycle_num[i];
    }
    // cout << endl;
    expr abs_back = m_abs_phi;
    Graph g_back = g_phi;
    RelationMatrix rm_back(m_phi_location_relation);

    vector<int> omega(cc_num, 0);
    expr omega_abs_i = m_abs_phi;
    expr omega_abs_i1(z3_ctx);
    Graph omega_g_i = g_phi;
    Graph omega_g_i1;
    // cout << "cc_num: " << cc_num <<endl;
    if (omega_g_i.isDagLike()) {
        if (matchGraph(omega_g_i, g_psi)) {
            return true;
        } else {
            return false;
        }
    }

    // showRM();

    do {
        bool is_feasible = true;
        if (cc_total > 0) {
            // cout << "omega: ";
            // for (int i=0; i<omega.size(); i++) {
            //     cout << omega[i] << " ";
            // }
            // cout << endl;
            getOmegaAbs(omega_abs_i, omega_g_i, omega, omega_abs_i1); // next omega
            omega_abs_i = omega_abs_i1;
            if (check(omega_abs_i, m_phi_free_items) == "SAT") {
                // feasible 
                m_abs_phi = omega_abs_i;
                // cout << "m_abs_phi: " << m_abs_phi <<endl;
                initRm(omega_abs_i, m_phi_location, m_phi_location_relation, m_phi_free_items);
                // showRM();
                constructPhiGraph(omega_g_i1);
                omega_g_i = omega_g_i1;
                m_counter++;
                string fname = "omega_";
                fname.append(to_string(m_counter));
                fname.append(".dot");
                expr space = m_phi.arg(m_phi.num_args()-1);
                omega_g_i.print(m_phi_location, space, fname);
                
                if (!omega_g_i.isDagLike()) {
                    // recursive
                    bool res = checkAllocatingPlans(omega_g_i, g_psi);
                    if (!res) return false;
                }
            } else {
                is_feasible = false;
            }
        }

        if (is_feasible) {
            // cout << "Matching ...." <<endl;
            if(!matchGraph(omega_g_i, g_psi)) {
                cout << "matching is failed!\n";
                return false;
            }
        }
        // next 
        omega_abs_i = abs_back;
        omega_g_i = g_back;
        m_abs_phi = abs_back;
        m_phi_location_relation.assign(rm_back.begin(), rm_back.end());
    } while (nextOmega(omega, cc_cycle_num));
    return true;
}


void SepSolver::getOmegaAbs(expr& phi_abs, Graph& g, vector<int>& omega, expr& omega_phi_abs) {
    //
    z3::expr m_phi = m_problem->getPhi();
    expr space = m_phi.arg(m_phi.num_args()-1);
    std::vector<int> cc_cycle_num = g.getCcCycleNum();
    std::pair<int,int> coords;
    std::vector<Graph::edge_t> cycle;
    int cc_num = cc_cycle_num.size();

    expr_vector and_items(z3_ctx);
    for (int i=0; i<cc_num; i++) {
        int omega_i = omega[i];
        if (omega_i==0) {
            // ! && !
            expr_vector not_and_items(z3_ctx);
            for (int j=0; j<cc_cycle_num[i]; j++) {
                coords.first = i;
                coords.second = j;
                cycle = g.getEdgeCycle(coords);
                int cycle_size = cycle.size();
                // edge
                for (int k=0; k<cycle_size; k++) {
                    Graph::edge_t edge = cycle[k];
                    int atom_idx = edge.second;
                    z3::expr E = space.arg(atom_idx).arg(0);
                    std::string E_bool_name = E.to_string().append("_BOOL_").append(to_string(atom_idx));
                    z3::expr E_bool = z3_ctx.bool_const(E_bool_name.c_str());
                    not_and_items.push_back(!E_bool);
                    // omega_phi_abs = omega_phi_abs && (!E_bool);
                }
            }
            and_items.push_back(mk_and(not_and_items));
        } else {
            //
            coords.first = i;
            coords.second = omega_i-1;
            cycle = g.getEdgeCycle(coords);
            int cycle_size = cycle.size();
            expr_vector or_items(z3_ctx);

            // edge
            for (int k=0; k<cycle_size; k++) {
                Graph::edge_t edge = cycle[k];
                int atom_idx = edge.second;
                z3::expr E = space.arg(atom_idx).arg(0);
                std::string E_bool_name = E.to_string().append("_BOOL_").append(to_string(atom_idx));
                z3::expr E_bool = z3_ctx.bool_const(E_bool_name.c_str());

                or_items.push_back(!E_bool);
            }
            and_items.push_back(!mk_and(or_items));
        }
    }
    // cout << "add items: " << mk_and(and_items) <<endl;
    omega_phi_abs = phi_abs && mk_and(and_items);
}


bool SepSolver::matchGraph(Graph& omega_g_i, Graph& g_psi) {
	expr m_phi = m_problem->getPhi();
	expr m_psi = m_problem->getPsi();
	
    //  get psi_edges
    std::vector<std::pair<std::pair<int, int>, int> > psi_edge_vec;
    g_psi.getEdges(psi_edge_vec);
    // get omega_edges
    std::vector<std::pair<std::pair<int, int>, int> > omega_edge_vec;
    omega_g_i.getEdges(omega_edge_vec);

    std::map<int, int> omega_edge_table;// all edges
    for (int i=0; i<omega_edge_vec.size(); i++) {
        omega_edge_table[omega_edge_vec[i].second] = -1;
    }

    //CC
    std::vector<int> cc_cycle_num = omega_g_i.getCcCycleNum();
    std::vector<std::pair<int, int> > cc_cycle_table;
    for (int i=0; i<cc_cycle_num.size(); i++) {
        std::pair<int, int> selected(-1, -1);
        if (cc_cycle_num[i] > 0) {
            selected.first = 0; // -1: no cycle; 0: no process; 1: candidate  2:selected
        }
        cc_cycle_table.push_back(selected);
    }

    std::vector<z3::expr> psi_const_vec = m_psi_location;
    std::vector<z3::expr> phi_const_vec = m_phi_location;
    z3::expr psi_space = m_psi.arg(m_psi.num_args()-1);
    z3::expr phi_space = m_phi.arg(m_phi.num_args()-1);

    std::pair<std::pair<int, int>, int> edge;
    // for each edge match one path in omega_graph
    for (int i=0; i<psi_edge_vec.size(); i++) {
        // cout <<"psi edge: "<< psi_const_vec[psi_edge_vec[i].first.first] << "--" << psi_edge_vec[i].second << "--" << psi_const_vec[psi_edge_vec[i].first.second] << std::endl;

        edge = psi_edge_vec[i];
        z3::expr psi_atom = psi_space.arg(edge.second);
        int src = z3_buffer.indexOf(phi_const_vec, psi_const_vec[edge.first.first]);
        int dst = z3_buffer.indexOf(phi_const_vec, psi_const_vec[edge.first.second]);

        src = omega_g_i.getVertexId(src);
        dst = omega_g_i.getVertexId(dst);

        // src and dst are in the same cc
        int cc_id = omega_g_i.whichCc(src);
        if (cc_id != omega_g_i.whichCc(dst)) return checkPsiPredEmpty(psi_atom); // path in different cc

        std::vector<Graph::edge_descriptor> path = omega_g_i.getPath(src, dst);

        cout << "path: \n";
        for (int j=0; j<path.size(); j++) {
            cout << omega_g_i.source(path[j]);
            cout << "---";
            cout << omega_g_i.getEdgeProperty(path[j]);
            cout << "---";
            cout << omega_g_i.target(path[j]) << std::endl;

        }

        std::vector<int> paths;
        for (int j=0; j<path.size(); j++) {
            paths.push_back(omega_g_i.getEdgeProperty(path[j]));
        }

        // special case
        if (paths.size()==0 && src!=dst) return checkPsiPredEmpty(psi_atom); 

        int edge_num = paths.size();
        if (psi_atom.decl().name().str() == "pto") {
            // pto match pto
            if (edge_num==1) {
                z3::expr omega_phi_atom = phi_space.arg(paths[0]);
                if (omega_phi_atom.decl().name().str() != "pto") {
                    return z3::unsat;
                }
                // match pto atom
                if(!matchPto(psi_atom, omega_phi_atom)) return false;
            } else {
                return false;
            }
        } else {
            // pred_atom match path
            if (cc_cycle_num[cc_id] == 0) {
                cout << "omega graph is dag. \n";
                // dag
                if(!matchPredicate(psi_atom, paths)) return false;
            } else {
                cout << "omega graph is dag-like. \n";
                // dag_like (each cc has at most one cycle)
                std::pair<int, int> coord(cc_id, 0);
                std::vector<int> cycle = omega_g_i.getCycle(coord);

                std::vector<Graph::edge_t> edge_cycle = omega_g_i.getEdgeCycle(coord);

                cout << "paths size: " << paths.size() << std::endl;
                // match paths
                bool match_res1  = matchPredicate(psi_atom, paths);
                cout << "match_res1: " << match_res1 <<endl;
                // match paths+cycle
                std::vector<Graph::edge_descriptor> cycle_path = omega_g_i.getPath(dst);
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
                    cout <<"cycle edge: "<< edge_cycle[j].first.first << "--" << (edge_cycle[j].second) << "--" << edge_cycle[j].first.second << std::endl;
                }

                bool match_res2 = true;

                if (edge_cycle.size() > 0 ) {
                    match_res2 = matchPredicate(psi_atom, paths);
                }

                cout << "match_res1: " << match_res1 << ", match_res2: " << match_res2 << std::endl;

                // whether dst and last_src are in cycle
                int last_src = -1;
                if (path.size() > 0) {
                    last_src = omega_g_i.source(path[path.size()-1]);
                }
                if (z3_buffer.indexOf(cycle, dst) != -1 && z3_buffer.indexOf(cycle, last_src) == -1) {
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
                } else if(z3_buffer.indexOf(cycle, dst) != -1 && src==dst){
                    // dst in, last_src in, src == dst
                    if (cc_cycle_table[cc_id].first != 2 && match_res2) {
                        cc_cycle_table[cc_id].first = 1;
                        cc_cycle_table[cc_id].second = i;
                    }
                } else {
                    if (!match_res1) return false;
                    if (z3_buffer.indexOf(cycle, dst) != -1) {
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
    cout <<"cc_cycle_table: \n";
    for (int i=0; i<cc_cycle_table.size(); i++) {

        cout << "cc_cycle_num: " << i << " , status: " << cc_cycle_table[i].first << std::endl;

        if (cc_cycle_table[i].first==0) return false;
        if (cc_cycle_table[i].first==1) {
            std::pair<int, int> coord(i, 0);
            std::vector<int> cycle = omega_g_i.getCycle(coord);
            std::vector<Graph::edge_descriptor> cycle_path = omega_g_i.getPath(cycle[0]);
            for (int j=0; j<cycle_path.size(); j++) {
                int path_idx = omega_g_i.getEdgeProperty(cycle_path[j]);
                omega_edge_table[path_idx]++;
            }
        }
    }

    //
    cout << "omega_edges_table: \n";

    std::map<int, int>::iterator iter;
    int i=0;
    for (iter=omega_edge_table.begin(); iter!=omega_edge_table.end(); iter++, i++) {
        cout << "edge: " << i <<", property: " <<iter->first << " , status: " << iter->second << std::endl;
        if (iter->second!=0) return false;
    }
    return true;
}


bool SepSolver::checkPsiPredEmpty(expr& psi_atom) {
    if (psi_atom.decl().name().str() == "pto") {
        return false;
    } else {
        // pred
        expr_vector data_items(z3_ctx);
        // abs_omega_phi |= pred atom is empty
        int pars_size = psi_atom.num_args();
        for (int i=0; i<pars_size/2; i++) {
            data_items.push_back(z3_buffer.mkEq(psi_atom.arg(i), psi_atom.arg(i+pars_size/2)));
        }
        cout << "data_itmes: " << data_items <<endl;
        if (data_items.size() > 0) {
            expr check_f = m_abs_phi && !z3::mk_and(data_items);
            if (check(check_f, m_phi_free_items) == "UNSAT") {
                cout << "match path is successful!\n";
                return true;
            }
            return false;
        }
        return true;
    }
}


bool SepSolver::matchPto(expr& psi_atom, expr& omega_atom) {
    // match pto --> pto
    std::vector<z3::expr> psi_vars;
    std::vector<z3::expr> phi_atom_vars;
    psi_vars.push_back(psi_atom.arg(0));
    phi_atom_vars.push_back(omega_atom.arg(0));
    for (int i=0; i<psi_atom.arg(1).num_args(); i++) {
        psi_vars.push_back(psi_atom.arg(1).arg(i));
        phi_atom_vars.push_back(omega_atom.arg(1).arg(i));
    }

    z3::expr_vector eq_items(z3_ctx);
    // match
    for (int i=0; i<psi_vars.size(); i++) {
        z3::expr psi_v = psi_vars[i];
        z3::expr phi_v = phi_atom_vars[i];

        if (psi_v.to_string() != phi_v.to_string()) {
            string sort = psi_v.get_sort().to_string();
            if (sort != "Bool" && sort != "Int" && sort != "SetInt") {
                eq_items.push_back(z3_ctx.int_const(psi_v.to_string().c_str()) ==
                       z3_ctx.int_const(phi_v.to_string().c_str()));
            } else {
                eq_items.push_back(psi_v == phi_v);
            }
        }
    }

    if (eq_items.size() > 0) {
        z3::expr check_f = m_abs_phi && !z3::mk_and(eq_items);
        z3::solver s(z3_ctx);
        if (s.check() == z3::sat) {
            return false;
        }
    }
    return true;
}


bool SepSolver::matchPredicate(expr& psi_atom, vector<int>& paths) {
    cout << "match Predicate: \n";
    // path match pred atom
    z3::expr m_phi = m_problem->getPhi();
    Predicate_SLID_SET *m_pred = dynamic_cast<Predicate_SLID_SET *>(m_problem->getPredicate());
    expr phi_space = m_phi.arg(m_phi.num_args()-1);
    z3::expr_vector data_items(z3_ctx);
    if (paths.size() == 0) {
        // abs_omega_phi |= pred atom is empty
        int pars_size = psi_atom.num_args();
        for (int i=0; i<pars_size/2; i++) {
            data_items.push_back(z3_buffer.mkEq(psi_atom.arg(i), psi_atom.arg(i+pars_size/2)));
        }
    } else {
        // divide path into several segments, continious ptos and continious predicates
        int psi_arg_size = psi_atom.num_args();

        vector<int> seg_start; // start, end,  flag:0,1[pto, pred]
        vector<int> seg_end; 
        vector<int> seg_flag;
        expr atom = phi_space.arg(paths[0]);
        int flag = 1;
        if (z3_buffer.isFun(atom, "pto")) {
            flag = 0;
        }
        seg_start.push_back(0);
        seg_flag.push_back(flag);

        for (int i=1; i<paths.size(); i++) {
            expr atom = phi_space.arg(paths[i]);
            expr atom_p = phi_space.arg(paths[i-1]);
            if (atom.decl().name().str() != atom_p.decl().name().str()) {
                flag = 1 - flag;
                seg_end.push_back(i);
                seg_start.push_back(i);
                seg_flag.push_back(flag);
            }
        }
        seg_end.push_back(paths.size());
        expr_vector seg_preds(z3_ctx);
        expr_vector args(z3_ctx);
        expr_vector args_2(z3_ctx);
        for (int i=0; i<psi_arg_size/2; i++) {
            args.push_back(psi_atom.arg(i));
        }
        for (int i=0; i<seg_start.size(); i++) {
            if (seg_flag[i] == 1) {
                // pred
                expr atom = phi_space.arg(paths[seg_end[i]-1]);
                for (int j=psi_arg_size/2; j<psi_arg_size; j++) {
                    args_2.push_back(atom.arg(j));
                }
            } else {
                // pto
                if (i == seg_start.size() - 1) {
                    for (int j=psi_arg_size/2; j<psi_arg_size; j++) {
                        args_2.push_back(psi_atom.arg(j));
                    }
                } else {
                    expr atom = phi_space.arg(paths[seg_start[i+1]]);
                    for (int j=0; j<psi_arg_size/2; j++) {
                        args_2.push_back(atom.arg(j));
                    }
                }
            }
            
            for(int j=0; j<args_2.size(); j++) {
                args.push_back(args_2[j]);
            }

            seg_preds.push_back(m_pred->apply(args));

            args.resize(0);
            for (int j=0; j<args_2.size(); j++) {
                args.push_back(args_2[j]);
            }
            args_2.resize(0);
        }
        // show segments 
        // for (int i=0; i<seg_start.size(); i++) {
        //     cout << seg_start[i] << "-" << seg_end[i] << ": " << seg_preds[i] <<endl;
        // }
        // generating conditions
        int seg_size = seg_preds.size();
        for (int i=0; i<psi_arg_size/2; i++) {
            data_items.push_back(z3_buffer.mkEq(psi_atom.arg(i), seg_preds[0].arg(i)));
            data_items.push_back(z3_buffer.mkEq(psi_atom.arg(i+psi_arg_size/2), 
                seg_preds[seg_size-1].arg(i+psi_arg_size/2)));
        }
        for (int i=0; i<seg_size; i++) {
            if (seg_flag[i] == 1) {
                // pred
                for (int j=seg_start[i]; j<seg_end[i]-1; j++) {
                    expr atom1 = phi_space.arg(paths[j]);
                    expr atom2 = phi_space.arg(paths[j+1]);
                    for (int k=0; k<psi_arg_size/2; k++) {
                        data_items.push_back(z3_buffer.mkEq(atom1.arg(k+psi_arg_size/2), 
                            atom2.arg(k)));
                    }
                }
            } else {
                // pto 
                expr pred = seg_preds[i];
                Z3ExprSet evars;
                expr_vector pevars(z3_ctx);
                expr_vector body_items(z3_ctx);
                for (int j=seg_start[i]; j<seg_end[i]; j++) {
                    // cout << "predicate: " << pred <<endl;
                    expr atom = phi_space.arg(paths[j]);
                    // unfold pred
                    expr_vector vars(z3_ctx);
                    for (int k=0; k<psi_arg_size/2; k++) {
                        if (k >0 && k <psi_arg_size/2-1) continue;
                        string name = pred.arg(k).to_string();
                        name.append("_ES");
                        expr v = z3_ctx.constant(name.c_str(), pred.arg(k).get_sort());
                        vars.push_back(v);
                        if (z3_buffer.isLocation(v)) {
                            evars.insert(z3_ctx.int_const(v.to_string().c_str()));
                        } else {
                            evars.insert(v);
                        }
                    }
                    expr unfoldp = m_pred->unfoldPredicate(vars);
                    expr_vector pars = m_pred->getPars();
                    expr_vector dst(z3_ctx);
                    for (int k=0; k<pred.num_args(); k++) {
                        dst.push_back(pred.arg(k));
                    }
                    unfoldp = unfoldp.substitute(pars, dst);
                    // cout << "unfoldp: " << unfoldp <<endl;
                    body_items.push_back(unfoldp.arg(0)); // data
                    expr pto = unfoldp.arg(1); // pto
                    body_items.push_back(z3_buffer.mkEq(atom.arg(0), pto.arg(0)));
                    expr fields = atom.arg(1);
                    for (int k=0; k<fields.num_args(); k++) {
                        body_items.push_back(z3_buffer.mkEq(fields.arg(k), pto.arg(1).arg(k)));
                    }
                    pred = unfoldp.arg(2); // new pred
                }
                for (int j=0; j<psi_arg_size/2; j++) {
                    body_items.push_back(z3_buffer.mkEq(pred.arg(j), pred.arg(j+psi_arg_size/2)));
                }
                for (auto v : evars) {
                    pevars.push_back(v);
                }
                // cout << "evars: " << pevars <<endl;
                // cout << "body: " << mk_and(body_items) <<endl;
                data_items.push_back(!forall(pevars, !mk_and(body_items)));
            }
        }

    }
    cout << "data_itmes: " << data_items <<endl;
    if (data_items.size() > 0) {
        expr check_f = m_abs_phi && !z3::mk_and(data_items);
        if (check(check_f, m_phi_free_items) == "UNSAT") {
            cout << "match path is successful!\n";
            return true;
        }
        return false;
    }
    return true;
}


bool SepSolver::_matchPredicate(expr& psi_atom, vector<int>& paths) {
    // path match pred atom
    z3::expr m_phi = m_problem->getPhi();
    Predicate_SLID_SET *m_pred = dynamic_cast<Predicate_SLID_SET *>(m_problem->getPredicate());
    expr phi_space = m_phi.arg(m_phi.num_args()-1);
    z3::expr_vector data_items(z3_ctx);
    if (paths.size() == 0) {
        // abs_omega_phi |= pred atom is empty
        int pars_size = psi_atom.num_args();
        for (int i=0; i<pars_size/2; i++) {
            data_items.push_back(z3_buffer.mkEq(psi_atom.arg(i), psi_atom.arg(i+pars_size/2)));
        }
    } else {
        // generate conditions
        int case_i = m_pred->getCase();
        int psi_arg_size = psi_atom.num_args();

        if (paths.size() == 1) {
            // pred -> pto or pred
            z3::expr phi_atom = phi_space.arg(paths[0]);

            if (z3_buffer.isFun(phi_atom, "pto")) {
                // left is pto
                for (int i=1; i<phi_atom.arg(1).num_args(); i++) {
                    if (z3_buffer.isLocation(phi_atom.arg(1).arg(i))) {
                        data_items.push_back(z3_buffer.mkEq(phi_atom.arg(1).arg(i), psi_atom.arg(i)));
                    } else {
                        data_items.push_back(phi_atom.arg(1).arg(i) == z3_buffer.getFirstElement(case_i, psi_atom.arg(i)));
                    }
                }
                // add data constraint
                data_items.push_back(m_pred->subPhiR2(psi_atom.arg(psi_arg_size/2-1), 
                    psi_atom.arg(psi_arg_size-1)));
            } else {
                // pred
                for (int i=0; i<psi_arg_size; i++) {
                    data_items.push_back(z3_buffer.mkEq(phi_atom.arg(i), psi_atom.arg(i)));
                }
            }
        } else {
            // path size >= 2

            int idx = m_pred->getEinGamma();
            // head
            expr atom0 = phi_space.arg(paths[0]);
            if (z3_buffer.isFun(atom0, "pto")) {
                // pto
                expr field_atom = atom0.arg(1);
                for (int i=1; i<field_atom.num_args(); i++) {
                    if (z3_buffer.isLocation(field_atom.arg(i))) {
                        data_items.push_back(z3_buffer.mkEq(field_atom.arg(i), psi_atom.arg(i)));
                    } else {
                        data_items.push_back(field_atom.arg(i) == z3_buffer.getFirstElement(case_i, psi_atom.arg(i)));
                    }
                }
            } else {
                // pred
                for (int i=1; i<psi_arg_size/2; i++) {
                    data_items.push_back(z3_buffer.mkEq(atom0.arg(i), psi_atom.arg(i)));
                }
            }
            // middle
            expr psi_S = psi_atom.arg(psi_arg_size/2-1);
            for (int k=0; k<paths.size()-1; k++) {
                expr atom1 = phi_space.arg(paths[k]);
                expr atom2 = phi_space.arg(paths[k+1]);
                bool is_pto1 = z3_buffer.isFun(atom1, "pto");
                bool is_pto2 = z3_buffer.isFun(atom2, "pto");

                if (is_pto1 && is_pto2) {
                    // pto pto
                    expr field1 = atom1.arg(1);
                    expr field2 = atom2.arg(1);
                    expr data1 = field1.arg(field1.num_args()-1);
                    expr data2 = field2.arg(field2.num_args()-1);
                    // dll 
                    if (idx != -1) {
                        data_items.push_back(z3_buffer.mkEq(field1.arg(0), field2.arg(idx)));
                    }

                    data_items.push_back(z3_buffer.getBelongsto(data1, psi_S));
                    data_items.push_back(z3_buffer.getBelongsto(data2, psi_S));
                    data_items.push_back(m_pred->subPhiR2(data1, data2));

                } else if (is_pto1 && !is_pto2) {
                    // pto pred
                    expr field1 = atom1.arg(1);
                    expr data1 = field1.arg(field1.num_args()-1);
                    expr atom2_S = atom2.arg(psi_arg_size/2-1);

                    // dll 
                    if (idx != -1) {
                        data_items.push_back(z3_buffer.mkEq(field1.arg(0), atom2.arg(idx+1)));
                    }
                    expr single_set = z3_buffer.getSet(data1);
                    expr set_union = z3_buffer.getSetunion(atom2_S, single_set);

                    data_items.push_back(z3_buffer.getSubset(set_union, psi_S));
                    data_items.push_back(m_pred->subPhiR2(data1, atom2_S));
                } else if (!is_pto1 && is_pto2) {
                    // pred pto
                    expr field2 = atom2.arg(1);
                    for (int i=1; i<field2.num_args(); i++) {
                        if (z3_buffer.isLocation(field2.arg(i))) {
                            data_items.push_back(z3_buffer.mkEq(field2.arg(i), atom2.arg(i)));
                        } else {
                            data_items.push_back(field2.arg(i) == z3_buffer.getFirstElement(case_i, atom1.arg(i+psi_arg_size/2)));
                            expr atom1_S = atom1.arg(i+psi_arg_size/2);
                            data_items.push_back(z3_buffer.getSubset(atom1_S, psi_S));
                        }
                    }

                    // dll 
                    if (idx != -1) {
                        data_items.push_back(z3_buffer.mkEq(field2.arg(0), atom1.arg(idx+1)));
                    }
                } else {
                    // pred pred
                    for (int i=1; i<psi_arg_size/2; i++) {
                        data_items.push_back(z3_buffer.mkEq(atom2.arg(i), atom1.arg(i+psi_arg_size/2)));
                        if (!z3_buffer.isLocation(atom2.arg(i))) {
                            expr atom2_S = atom2.arg(i);
                            data_items.push_back(z3_buffer.getSubset(atom2_S, psi_S));
                        }
                    }
                }
            }

            // tail
            expr atom_tail = phi_space.arg(paths[paths.size()-1]);
            if (z3_buffer.isFun(atom_tail, "pto")) {
                // pto
                // NONE
            } else {
                // pred
                for (int i=psi_arg_size/2+1; i<psi_arg_size; i++) {
                    data_items.push_back(z3_buffer.mkEq(atom_tail.arg(i), psi_atom.arg(i)));
                }
            }
        }
    }
    cout << "data_items: " << data_items << std::endl;
    if (data_items.size() > 0) {
        expr check_f = m_abs_phi && !z3::mk_and(data_items);
        if (check(check_f, m_phi_free_items) == "UNSAT") {
            return true;
        }
        return false;
    }
    return true;
}


void SepSolver::constructPhiGraph(Graph& g) {
    _constructPhiGraph(g, m_phi_location_relation);
}

void SepSolver::_constructPhiGraph(Graph& g, RelationMatrix& rm) {
	expr m_phi = m_problem->getPhi();
	
    // compute eq class by rm
    _computeEqClass(rm, m_phi_location_eq_class, m_phi_eq_class);
    // generate edges
    expr space = m_phi.arg(m_phi.num_args()-1);
    vector<EdgeType> edges;
    for (int i=0; i<space.num_args(); i++) {
        EdgeType edge;
        edge.second = i;
        expr atom = space.arg(i);
        if (atom.decl().name().str() != "pto") {
            //if (m_phi_space_allocated[i] == 0) {
            //    cout << atom << " no allocated!" <<endl;
            //    continue;
            // }

            string name = atom.arg(0).to_string();
            name = name.append("_BOOL_").append(to_string(i)); 
            expr new_bool = z3_ctx.bool_const(name.c_str());
            // cout << "edge: " << new_bool <<endl;
            expr check_f = m_abs_phi && new_bool;
            string res = check(check_f, m_phi_free_items);
            if (res == "UNSAT") {
                // cout << "invalid...\n";
                continue;
            }
        }
        atomToEdge(atom, edge, m_phi_location);
        edges.push_back(edge);
    }

    // cout << "edge number: " << edges.size() <<endl;

    g.init(m_phi_eq_class, edges);
}


void SepSolver::printPhi(Graph& g_phi, string fname) {
	expr m_phi = m_problem->getPhi();
    expr space = m_phi.arg(m_phi.num_args()-1);
    g_phi.print(m_phi_location, space, fname);
}


void SepSolver::constructPsiGraph(Graph& g) {
	expr m_psi = m_problem->getPsi();
    // compute eq class by rm
    _computeEqClass(m_psi_location_relation, m_psi_location_eq_class, m_psi_eq_class);
    // generate edges
    expr space = m_psi.arg(m_psi.num_args()-1);
    vector<EdgeType> edges;
    for (int i=0; i<space.num_args(); i++) {
        EdgeType edge;
        edge.second = i;
        expr atom = space.arg(i);
        if (atom.decl().name().str() != "pto") {
            string name = atom.arg(0).to_string();
            name = name.append("_BOOL_").append(to_string(i)); 
            expr new_bool = z3_ctx.bool_const(name.c_str());
            expr check_f = m_abs_psi && new_bool;
            string res = check(check_f, m_psi_free_items);
            if (res == "UNSAT") continue;
        }
        atomToEdge(atom, edge, m_psi_location);
        edges.push_back(edge);
    }

    g.init(m_psi_eq_class, edges);
    g.print(m_psi_location, space, "graph_psi.dot");
}

void SepSolver::atomToEdge(expr& atom, EdgeType& edge, vector<expr>& locations) {

    expr src = atom.arg(0);
    expr dst(z3_ctx);
    if (atom.decl().name().str() == "pto") {
        dst = atom.arg(1).arg(0);
    } else {
        dst = atom.arg(atom.num_args()/2);
    }
    edge.first.first = z3_buffer.indexOf(locations, src);
    edge.first.second = z3_buffer.indexOf(locations, dst);
}

bool SepSolver::nextOmega(vector<int>& curr, vector<int>& target) {
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
