#include "solver_slid_int/alistsolver.h"
//extern z3::context z3_ctx;

bool alistsolver::match_graph(listgraph& g_psi, listgraph& omega_g_i) {
     // TODO (jackchong#1#06/18/18): compute allocated-table ...

    // m_ctx.phi_abs + omega_g_i -> allocated_table:
    // eq, neq table:

    std::cout << "alist:: match_graph!!!\n";

    // logger() << "phi_abs: " << m_ctx.phi_abs << std::endl;

    std::cout << "alist:: match graph\n";


    int eq_size = m_omega_eq_size;
    int const_size = m_phi_const_eq_class_vec.size();
    std::cout << "equality class size: " << eq_size << std::endl;

    std::vector<std::vector<int> > eq_to_const_vec(eq_size);
    for (int i=0; i<const_size; i++) {
        eq_to_const_vec[m_phi_const_eq_class_vec[i]].push_back(i);
    }

    int nil_eq = m_phi_const_eq_class_vec[0];

    std::cout << "eq class: \n";

    for (int i=0; i<eq_size; i++) {
        std::cout << i <<": ";
        for (int j=0; j<eq_to_const_vec[i].size(); j++) {
            std::cout << m_phi_const_vec[eq_to_const_vec[i][j]] << " ";
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;

    z3::solver s(z3_ctx);
    bool allocated_table[eq_size]; // location variable allocated
    for (int i=0; i<eq_size; i++) {
        int vid = omega_g_i.get_vertex_id(eq_to_const_vec[i][0]);
        z3::expr_vector neg_vec(z3_ctx);
        std::vector<listgraph::edge_descriptor> rc_edges = omega_g_i.get_reachable_edges(vid);
        for (int j=0; j<rc_edges.size(); j++) {
            int idx = omega_g_i.get_edge_property(rc_edges[j]);
            std::string source_name = "["+m_phi_space.arg(idx).arg(0).to_string()+","+to_string(idx)+"]";
            neg_vec.push_back(!z3_ctx.bool_const(source_name.c_str()));
        }
        s.reset();
        z3::expr neg_cf = z3::mk_and(neg_vec);
        z3::expr check_f = m_phi_abs && neg_cf;
        s.add(check_f);
        if (s.check() == z3::unsat) {
            allocated_table[i] = true;
        } else {
            allocated_table[i] = false;
        }
    }

    std::cout << "eq class allocated table: \n";

    for (int i=0; i<eq_size; i++) {
        std::cout << i <<": " << allocated_table[i] << std::endl;
    }
    std::cout << std::endl;


    bool neq_table[eq_size][eq_size]; // equality class neq relation


    for (int i=0; i<eq_size; i++) {
        neq_table[i][i] = false;
        for (int j=i+1; j<eq_size; j++) {
            int idx_x = eq_to_const_vec[i][0];
            z3::expr X = z3_ctx.int_const(m_phi_const_vec[idx_x].to_string().c_str());
            int idx_y = eq_to_const_vec[j][0];
            z3::expr Y = z3_ctx.int_const(m_phi_const_vec[idx_y].to_string().c_str());
            s.reset();
            s.add(m_phi_abs && (X == Y));
            if (s.check() == z3::unsat) {
                neq_table[i][j] = true;
                neq_table[j][i] = true;
            } else {
                neq_table[i][j] = false;
                neq_table[j][i] = false;
            }
        }
    }

    std::cout << "neq table: \n";
    for (int i=0; i<eq_size; i++) {
        for (int j=0; j<eq_size; j++) {
            std::cout << "\t" << neq_table[i][j];
        }
        std::cout << std::endl;
    }
    std::cout << std::endl;



    //  get psi_edges
    std::vector<std::pair<std::pair<int, int>, int> > psi_edge_vec;
    g_psi.get_edges(psi_edge_vec);
     /*
    for (int i=0; i<psi_edge_vec.size(); i++) {
        logger() <<"psi edge: "<< m_ctx.psi_const_vec[psi_edge_vec[i].first.first]
            << "--" << psi_edge_vec[i].second << "--"
            << m_ctx.psi_const_vec[psi_edge_vec[i].first.second] << std::endl;
    }
    */
    // get omega_edges
    std::vector<std::pair<std::pair<int, int>, int> > omega_edge_vec;
    omega_g_i.get_edges(omega_edge_vec);
    /*
    for (int i=0; i<omega_edge_vec.size(); i++) {
        logger() <<"omega edge: "<< m_ctx.phi_const_vec[omega_edge_vec[i].first.first]
            << "--" << omega_edge_vec[i].second << "--"
            << m_ctx.phi_const_vec[omega_edge_vec[i].first.second] << std::endl;
    }
    */
    // std::vector<int> omega_edge_table(omega_edge_vec.size(), -1);

    std::vector<int> omega_edge_table(m_phi_space.num_args());
    for (int i=0; i<omega_edge_vec.size(); i++) {
            omega_edge_table[omega_edge_vec[i].second] = -1;
    }

    // get nodes set reach nil
    std::set<size_t> nil_set = omega_g_i.get_reachable_set(nil_eq);
    std::cout << "reach nil set: \n";
    for (auto i : nil_set) {
        std::cout << i << " ";
    }
    std::cout << std::endl;

    //CC
    std::vector<int> cc_cycle_num = omega_g_i.get_cc_cycle_num();
    std::vector<std::set<int> > cc_cycle_intersection(cc_cycle_num.size());// intersection
    std::vector<std::set<int> > cc_cycle_set(cc_cycle_num.size());
    for (int i=0; i<cc_cycle_num.size(); i++) {
        if (cc_cycle_num[i] != 0) {
            std::pair<int, int> coord(i, 0);
            std::vector<int> cycle = omega_g_i.get_cycle(coord);
            std::cout << "cycle: ";
            for (int j=0; j<cycle.size(); j++) {
                std::cout << " " << cycle[j];
                // logger() << m_ctx.phi_const_vec[eq_to_const_vec[cycle[j]][0]] << " ";
                cc_cycle_set[i].insert(cycle[j]);
            }
            std::cout << std::endl;
        }
    }
    // exit(-1);




    std::pair<std::pair<int, int>, int> edge;
    for (int i=0; i<psi_edge_vec.size(); i++) {
        std::cout <<"psi edge: "<< m_psi_const_vec[psi_edge_vec[i].first.first]
            << "--" << psi_edge_vec[i].second << "--"
            << m_psi_const_vec[psi_edge_vec[i].first.second] << std::endl;

        edge = psi_edge_vec[i];

        z3::expr psi_atom = m_psi_space.arg(edge.second);

        int src = expr_tool::index_of_exp(m_psi_const_vec[edge.first.first], m_phi_const_vec);
        int dst = expr_tool::index_of_exp(m_psi_const_vec[edge.first.second], m_phi_const_vec);

        src = omega_g_i.get_vertex_id(src);
        dst = omega_g_i.get_vertex_id(dst);
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

        if (expr_tool::is_fun(psi_atom, "pto")) {
            // pto ->  pto
            std::cout << "psi pto match path.\n";
            if (path.size() > 1) {
                std::cout << "psi pto: mismatch path!\n";
                return false;
            } else {
                int idx = omega_g_i.get_edge_property(path[0]);
                if (expr_tool::is_fun(m_phi_space.arg(idx), "pto")) {

                } else {
                    std::cout << "psi pto: mismatch predicate atom.\n";
                    return false;
                }
            }

        } else {
             // match psi-atom -> phi-path
             if (src == dst) continue;

              // pred_atom match path
            if (cc_cycle_num[cc_id] == 0) {
                    std::cout << "omega graph is dag. \n";
                    // dag
                    //  pred-atom match paths in dag ...

                    if (path.size() == 1) {
                        int idx = omega_g_i.get_edge_property(path[0]);
                        int src_eq = omega_g_i.source(path[0]);
                        int dst_eq = omega_g_i.target(path[0]);
                        if (expr_tool::is_fun(m_phi_space.arg(omega_g_i.get_edge_property(path[0])), "pto")) {

                            if (!neq_table[src_eq][dst_eq]) {
                                std::cout << "psi pred: pto incomplete neq relation.\n";
                                return false; // pto not neq relation
                            }
                        }
                    } else if(!allocated_table[dst]) {
                        //
                        if (nil_set.find(dst) == nil_set.end()) {
                            int j=0;
                            for (; j<path.size()-1; j++) {
                                int idx = omega_g_i.get_edge_property(path[j]);
                                int src_eq = omega_g_i.source(path[j]);
                                // int dst_eq = omega_g_i.target(path[j]);
                                if (expr_tool::is_fun(m_phi_space.arg(idx), "pto")) {
                                    if (!neq_table[src_eq][dst]) {
                                        std::cout << "psi pred: pto+pto+pto: incomplete neq relations.\n";
                                        return false; // pto + ... + (pto) -> incomplete neq relations -> match
                                    }
                                } else {
                                    std::cout << "psi pred: ... + ls + ... + \n";
                                    return false; // pred-edge: ... + ls + ... + -> mismatch
                                }
                            }
                            // the last edge
                            int idx = omega_g_i.get_edge_property(path[j]);
                            int src_eq = omega_g_i.source(path[j]);
                            if (expr_tool::is_fun(m_psi_space.arg(idx), "pto")) {
                                if (!neq_table[src_eq][dst]) {
                                    std::cout << "psi pred: pto+pto+pto: incomplete neq relations.\n";
                                    return false; // pto + ... + (pto) -> incomplete neq relations -> match
                                }
                            }
                        }
                    }
            } else {
                std::cout << "omega graph is dag-like. \n";
                // dag_like (each cc has at most one cycle)

                // pred-atom math path in dag-like
                // cycle-join-nodes: cycle -> nodes-set
                // paths + cycle -> nodes
                 std::set<int>& cycle = cc_cycle_set[cc_id];
                 if (cycle.find(dst) != cycle.end()) {
                        bool flag = false;
                        for (int j=0; j<path.size(); j++) {
                            int source = omega_g_i.source(path[j]);
                            if (cycle.find(source) != cycle.end()) {
                                cc_cycle_intersection[cc_id].insert(source);
                                flag = true;
                                break;
                            }
                        }
                        if (flag) {
                            cc_cycle_intersection[cc_id].insert(dst);
                        }
                 }
            }
        }
        for (int j=0; j<path.size(); j++) {
            omega_edge_table[omega_g_i.get_edge_property(path[j])] ++;
        }
    }
    //
    std::cout << "cycle intersection: \n";

    for (int i=0; i<cc_cycle_intersection.size(); i++) {
        for (auto j : cc_cycle_intersection[i]) {
            int idx = eq_to_const_vec[j][0];
            std::cout << "  " << m_phi_const_vec[idx];
        }
        std::cout << std::endl;
        // check
        if (cc_cycle_num[i] > 0) {
            std::set<int>::iterator it1 = cc_cycle_intersection[i].begin();
            std::set<int>::iterator it = cc_cycle_intersection[i].end();
            std::set<int>::iterator it2;
            bool pass = false;
            for (; !pass && it1 != it; it1++) {
                    it2 = it1;
                    it2++;
                for (; !pass && it2 != it; it2++) {
                    std::cout << "    " << *it1 << " " << *it2 << std::endl;
                    if (neq_table[*it1][*it2]) {
                        pass = true;
                    }
                }
            }
            if (!pass) {
                std::cout << "intersection: no eq relations\n";
                return false;
            }
        }
    }

    std::cout << "cover omega edge table: \n";
    for (int i=0; i<omega_edge_vec.size(); i++) {
            std::cout << omega_edge_table[omega_edge_vec[i].second] << " ";

            if (omega_edge_table[omega_edge_vec[i].second] == -1) {
                return false;
            }
    }
    std::cout << std::endl;

    return true;
}


z3::expr alistsolver::pred_unfold_ge1(z3::expr& atom, int i, z3::expr_vector& new_bools) {
    Predicate_SLID_INT *pred = dynamic_cast<Predicate_SLID_INT *>(m_problem->getPredicate()); 
    int size = atom.num_args() - pred->size_of_static_parameters(); // size of source and destination paramaters

    z3::expr src = z3_ctx.int_const(atom.arg(0).to_string().c_str());
    z3::expr dst = z3_ctx.int_const(atom.arg(size/2).to_string().c_str());

    z3::expr or_ge1 = listsolver::pred_unfold_ge1(atom, i, new_bools);

    return (src != dst) && or_ge1;
}
