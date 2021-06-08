#include "solver_slid_set/FA.h"
#include <iostream>
#include <fstream>
#include <random>

//extern z3::context z3_ctx;
//extern Z3Buffer z3_buffer;


FA::FA(const FA& other):z3_ctx(other.z3_ctx), z3_buffer(other.z3_buffer) {
    m_init_state = other.m_init_state;
    m_state_num = other.m_state_num;
    m_accept_states.clear();
    setAcceptStates(other.m_accept_states);
    m_alphabet.clear();
    setAlphabetSet(other.m_alphabet);
    m_fa = other.m_fa;
}

FA& FA::operator=(const FA& other) {
    if (this != &other) {
    	//z3_ctx = other.z3_ctx;
		//z3_buffer = other.z3_buffer;
        m_init_state = other.m_init_state;
        m_state_num = other.m_state_num;
        m_accept_states.clear();
        setAcceptStates(other.m_accept_states);
        m_alphabet.clear();
        setAlphabetSet(other.m_alphabet);
        m_fa = other.m_fa;
    }
    return *this;
}


void FA::init() {
    std::cout << "init automata.\n";
}

void FA::setAlphabetSet(const std::vector<std::string> &alphabet)  {
    m_alphabet.insert(m_alphabet.end(), alphabet.begin(), alphabet.end());
    for (int i=0; i<m_alphabet.size(); i++) {
        m_var_to_pos[m_alphabet[i]] = i;
        //std::cout << m_alphabet[i] << ": " <<  m_var_to_pos[m_alphabet[i]] << std::endl;
    }
}

void FA::addStates(int n, std::string prefix) {
    m_state_num = n;
    for (int i=0; i<n; i++) {
        boost::add_vertex(m_fa);
        std::string name = prefix;
        name.append(std::to_string(i));
        m_fa[i].push_back(name);
        std::string key = vecToStr(m_fa[i]);
        m_fa[boost::graph_bundle][key] = i;
    }
}


void FA::addTransition(transition& tr) {
    boost::add_edge(tr.src, tr.dst, tr.info, m_fa);
}

/**
 * interset this and other
 * @param other
 * @param result
 */
FA FA::product(FA& other) {
    FA result(z3_ctx, z3_buffer);
    if (!isSameAlphabet(other)) {
        std::cout << "THE ALPHABET IS NOT SAME!\n";
        exit(-1);
    }

    result.setAlphabetSet(m_alphabet);
    result.m_state_num = m_state_num * other.m_state_num;
    // accept states
    for (int i=0; i<m_accept_states.size(); i++) {
        for (int j=0; j<other.m_accept_states.size(); j++) {
            result.m_accept_states.push_back(m_accept_states[i]*other.m_state_num + other.m_accept_states[j]);
        }
    }
    // result.m_accept_states.push_back(result.m_state_num-1);
    // add states
    int index = 0;
    for (int i=0; i<m_state_num; i++) {
        for (int j=0; j<other.m_state_num; j++) {
            boost::add_vertex(result.m_fa);
            result.m_fa[index].insert(result.m_fa[index].end(), m_fa[i].begin(), m_fa[i].end());
            result.m_fa[index].insert(result.m_fa[index].end(), other.m_fa[j].begin(), other.m_fa[j].end());
            std::string key = vecToStr(result.m_fa[index]);
            result.m_fa[boost::graph_bundle][key] = index;
            index++;
        }
    }
    // add transitions
    std::vector<int> work_list;
    std::set<int> state_set;
    work_list.push_back(0);

    while(work_list.size() != 0) {
        int cur_state = work_list.back();
        // std::cout << "cur_state: " << cur_state << std::endl;
        work_list.pop_back();

        if (state_set.find(cur_state) != state_set.end()) {
            continue;
        }
        state_set.insert(cur_state);

        int i = cur_state / other.m_state_num;
        int j = cur_state % other.m_state_num;

        automata::out_edge_iterator i_start, i_end;
        tie(i_start, i_end) = boost::out_edges(i, m_fa);
        for (; i_start != i_end; ++i_start) {
            // edge : i-> i_target
            int i_target = boost::target(*i_start, m_fa);
            std::vector<std::string> i_edge = m_fa[*i_start];

            automata::out_edge_iterator j_start, j_end;
            tie(j_start, j_end) = boost::out_edges(j, other.m_fa);
            for(; j_start != j_end; ++j_start) {
                // edge: j->j_target
                int j_target = boost::target(*j_start, other.m_fa);
                std::vector<std::string> j_edge = other.m_fa[*j_start];
                std::vector<std::string> r_edge;
                if (intersetEdge(i_edge, j_edge, r_edge)) {
                    int r_target = i_target*other.m_state_num + j_target;
                    if (state_set.find(r_target) == state_set.end()) {
                        work_list.push_back(r_target);
                    }
                    // r_src -> r_target : r_edge
                    // std::cout << "add edge: " << cur_state << "->" << r_target << std::endl;
                    boost::add_edge(cur_state, r_target, r_edge, result.m_fa);
                }
            }
        }

    }

    return result;
}

/**
 * state -> edge
 * get new FA
 * @return
 */
FA FA::stateAsEdge() {
    FA result(z3_ctx, z3_buffer);
    result.m_init_state = 0;
    result.m_state_num = m_state_num + 1;
    for (int i=0; i<m_accept_states.size(); i++) {
        result.m_accept_states.push_back(m_accept_states[i]+1);
    }

    result.addStates(result.m_state_num, "q");

    // int edge_count = 0;
    // add transition
    boost::add_edge(0, 1, m_fa[0], result.m_fa);
    for (int i=0; i<m_state_num; i++) {
        std::set<int> target_set;
        automata::out_edge_iterator i_start, i_end;
        tie(i_start, i_end) = boost::out_edges(i, m_fa);
        for (; i_start != i_end; ++i_start) {
            int i_target = boost::target(*i_start, m_fa);
            target_set.insert(i_target);
        }

        for (int i_target : target_set) {
            boost::add_edge(i+1, i_target+1, m_fa[i_target], result.m_fa);
            std::string key = vecToStr(m_fa[i_target]);
            result.m_fa[boost::graph_bundle][key] = m_fa[boost::graph_bundle][key];
        }
    }
    // std::cout << "state_count: " << result.m_state_num << std::endl;
    // std::cout << "edge_count: " << edge_count << std::endl;
    return result;
}

/**
 * get subgraph of G
 */
FA FA::getSubgraph(int N) {
    FA result(z3_ctx, z3_buffer);

    std::set<int> valid_ids;
    getValidStates(m_accept_states[0], valid_ids);

    std::default_random_engine generator;


    result.setAlphabetSet(m_alphabet);
    std::set<int> visited;
    std::vector<int> work_list;
    work_list.push_back(0);

    std::vector<transition> transitions;

    int edge_count = 0;

    while(work_list.size() > 0) {
        int cur_state = work_list.back();
        work_list.pop_back();
        if (visited.find(cur_state) == visited.end()) {
            // forward search
            automata::out_edge_iterator i_start, i_end;

            tie(i_start, i_end) = boost::out_edges(cur_state, m_fa);

            int i = 0;
            // std::cout << "cur: " << cur_state << " select: " ;

            for(; i_start != i_end; ++i_start) {
                int i_dst = boost::target(*i_start, m_fa);

                edge_count ++;
                // std::cout << cur_state << " -> " << i_dst << std::endl;

                if (valid_ids.find(i_dst) != valid_ids.end() &&
                    (i<N || i_dst == cur_state)) {

                    // std::cout << " " << i_dst;
                    work_list.push_back(i_dst);

                    transition tr;
                    tr.src = cur_state;
                    tr.dst = i_dst;
                    tr.info = m_fa[*i_start];
                    transitions.push_back(tr);
                    if (i_dst != cur_state && visited.find(i_dst) == visited.end()) i++;
                }
            }
            // std::cout << std::endl;
            visited.insert(cur_state);
        }
    }

    // add vertex
    result.addStates(m_state_num, "q_");

    // std::cout << "edge_count: " << edge_count << std::endl;

    // std::cout << "sub edge_count: " << transitions.size() << std::endl;

    // add transitions
    for (int i=0; i<transitions.size(); i++) {
        transition tr = transitions[i];
        result.addTransition(tr);
    }

    // set info
    result.m_init_state = m_init_state;
    result.setAcceptStates(m_accept_states);

    // bundle
    std::map<std::string, int> bundles =  m_fa[boost::graph_bundle];
    std::map<std::string, int>::iterator it = bundles.begin();
    while( it != bundles.end()) {
        result.m_fa[boost::graph_bundle][it->first] = it->second;
        it++;
    }

    return result;

}


/**
 * get flow as NFA
 */
FA FA::getFlow() {
    FA result(z3_ctx, z3_buffer);

    // accept_states size > 1
    if (m_accept_states.size() > 1) {
        // add new state
        boost::add_vertex(m_fa);
        int new_accept = m_state_num;
        m_state_num++;
        for (int i=0; i<m_accept_states.size(); i++) {
            int accept = m_accept_states[i];
            automata::in_edge_iterator i_start, i_end;
            tie(i_start, i_end) = boost::in_edges(accept, m_fa);
            for (; i_start != i_end; i_start++) {
                int src = boost::source(*i_start, m_fa);
                // add new edge
                boost::add_edge(src, new_accept, m_fa[*i_start], m_fa);
            }
        }
        // change m_accept_states
        m_accept_states.clear();
        m_accept_states.push_back(new_accept);
    }

    // std::cout << "add one accept.\n";

    int accept = m_accept_states[0];

    result.setAlphabetSet(m_alphabet);
    std::set<int> visited;
    std::vector<int> work_list;
    work_list.push_back(accept);

    std::vector<transition> transitions;

    int edge_count = 0;
    while(work_list.size() > 0) {
        int cur_state = work_list.back();
        work_list.pop_back();
        if (visited.find(cur_state) == visited.end()) {
            visited.insert(cur_state);
            // forward search
            automata::in_edge_iterator i_start, i_end;
            tie(i_start, i_end) = boost::in_edges(cur_state, m_fa);
            for(; i_start != i_end; ++i_start) {
                int i_src = boost::source(*i_start, m_fa);


                if (visited.find(i_src) == visited.end()) {
                    work_list.push_back(i_src);
                }
                transition tr;
                tr.src = i_src;
                tr.dst = cur_state;
                tr.info = m_fa[*i_start];
                transitions.push_back(tr);
                edge_count++;
            }
        }
    }

    // add vertex
    result.addStates(visited.size(), "q_");


    // add transitions
    int* id_map = new int[m_state_num];
    id_map[0] = 0;
    int i = 1;
    for (int cur : visited) {
        if (cur != 0 && cur != accept) {
            id_map[cur] = i++;
        }
    }
    id_map[accept] = i;
    for (i=0; i<transitions.size(); i++) {
        transition tr = transitions[i];
        tr.src = id_map[tr.src];
        tr.dst = id_map[tr.dst];
        result.addTransition(tr);
    }

    // set info
    result.m_init_state = m_init_state;
    std::vector<int> accept_states;
    accept_states.push_back(id_map[accept]);
    result.setAcceptStates(accept_states);

    // std::cout << "flow state size: " << visited.size() << std::endl;
    // std::cout << "flow edge count: " << edge_count << std::endl;

    delete id_map;
    return result;
}


/**
 * get valid ids which can reach accept
 */
void FA::getValidStates(int accept_state, std::set<int>& valid_ids) {

    std::vector<int> work_list;
    work_list.push_back(accept_state);

    while(work_list.size() > 0) {
        int cur = work_list.back();
        work_list.pop_back();
        if (valid_ids.find(cur) == valid_ids.end()) {
            valid_ids.insert(cur);
            automata::in_edge_iterator i_start, i_end;
            tie(i_start, i_end) = boost::in_edges(cur, m_fa);
            for (; i_start != i_end; i_start++) {
                int src = boost::source(*i_start, m_fa);
                if (valid_ids.find(src) == valid_ids.end()) {
                    work_list.push_back(src);
                }
            }
        }
    }
}


/**
 * pre: PA is special: one init state, one accept state
 * PA -> expr
 * expr = consitent_phi and connected_phi and sigma_item
 * @param accept_state : accept state id
 * @param x_ids : flow related to new var x_ids
 */
z3::expr FA::toExpr(z3::context& ctx, int accept_state, std::set<int>& x_ids, std::set<z3::expr, exprcomp>& tpaq_set) {
    z3::expr result = ctx.bool_val(true);

    std::set<int> valid_ids;
    getValidStates(accept_state, valid_ids);
    // std::cout << "valid_ids: " << valid_ids.size() << std::endl;

    // consitent
    std::map<int, std::vector<z3::expr> > xi_to_tpaq; // x_i = \Sigma (x_paq)
    // init

    z3::expr_vector consistent_items(ctx); // consistent_items
    z3::expr_vector connected_items(ctx); // conneted_items

    std::vector<z3::expr> in_flow_vec;

    std::map<int, int> id_to_pos; // in in_flow_vec
    std::map<int, std::map<int, std::vector<z3::expr> > > id_to_neighbors;

    std::vector<int> work_list;
    work_list.push_back(accept_state);


    std::set<int> visited;
    while(work_list.size() > 0) {
        int i = work_list.back();
        work_list.pop_back();
        if (visited.find(i) != visited.end()) {
            continue;
        }
        visited.insert(i);
        automata::in_edge_iterator i_start, i_end;
        tie(i_start, i_end) = boost::in_edges(i, m_fa);
        z3::expr_vector in_items(ctx);

        if (i==0) {
            in_items.push_back(ctx.int_val(1));
        }
        for (; i_start != i_end; ++i_start) {
            int i_src = boost::source(*i_start, m_fa);

            if (visited.find(i_src) == visited.end()) {
                work_list.push_back(i_src);
            }

            std::string key = vecToStr(m_fa[*i_start]);
            int i_edge = m_fa[boost::graph_bundle][key];

            // make var t_p_a_q
            z3::expr t_p_a_q = z3_buffer.mkIntVar("t_", i_src, i_edge, i);
            in_items.push_back(t_p_a_q);

            tpaq_set.insert(t_p_a_q);


            // id_to_neigbors
            if (i_src != i) {
                id_to_neighbors[i][i_src].push_back(t_p_a_q);
            }

            // insert x_i

            xi_to_tpaq[i_edge].push_back(t_p_a_q);

            x_ids.insert(i_edge); // add x_id




        }


        automata::out_edge_iterator o_start, o_end;
        tie(o_start, o_end) = boost::out_edges(i, m_fa);


        z3::expr_vector out_items(ctx);
        if (i==accept_state) {
            out_items.push_back(ctx.int_val(1));
        }
        for (; o_start != o_end; ++o_start) {
            int i_target = boost::target(*o_start, m_fa);

            if (valid_ids.find(i_target) != valid_ids.end()) {
                std::string key = vecToStr(m_fa[*o_start]);
                int i_edge = m_fa[boost::graph_bundle][key];
                // make var t_p_a_q
                z3::expr t_p_a_q = z3_buffer.mkIntVar("t_", i, i_edge, i_target);

                out_items.push_back(t_p_a_q);

                // tpaq_set.insert(t_p_a_q);

                // id_to_neighbors
                if (i_target != i) {
                    id_to_neighbors[i][i_target].push_back(t_p_a_q);
                }

            }
        }

        if (in_items.size() == 0) {
            std::cout << "state: " << i << ", indegree is zero\n";
            exit(-1);
        }
        if (out_items.size() == 0) {
            std::cout << "state: " << i << ", outdegree is zero\n";
            exit(-1);
        }

        z3::expr in_item = z3::sum(in_items);

        id_to_pos[i] = in_flow_vec.size();
        in_flow_vec.push_back(in_item);

        z3::expr i_item = (in_item == z3::sum(out_items));
        consistent_items.push_back(i_item);

    }




    std::map<int, std::map<int, std::vector<z3::expr> > >::iterator it = id_to_neighbors.begin();
    while(it != id_to_neighbors.end()) {
        int i = it->first;
        z3::expr u_i = z3_buffer.mkIntVar("u_", i);
        if (i != 0) {
            z3::expr pre = in_flow_vec[id_to_pos[i]] > 0;
            z3::expr_vector pos_or_items(ctx);

            std::map<int, std::vector<z3::expr> > neighbors = it->second;
            std::map<int, std::vector<z3::expr> >::iterator it1 = neighbors.begin();
            while(it1 != neighbors.end()) {
                int j = it1->first;
                z3::expr u_j = z3_buffer.mkIntVar("u_", j);
                z3::expr in_f =  in_flow_vec[id_to_pos[j]];
                //if (j == 0) {
                //    pos_or_items.push_back(u_i > u_j);
                //} else {
                z3::expr_vector or_t_paq_items(ctx);
                std::vector<z3::expr> t_paq_vec = it1->second;
                for (int k=0; k<t_paq_vec.size(); k++) {
                    or_t_paq_items.push_back(t_paq_vec[k] > 0);
                }
                pos_or_items.push_back(z3::mk_or(or_t_paq_items) && u_i > u_j);
                //}
                it1++;
            }
            connected_items.push_back(z3::implies(pre,  u_i > 0 && z3::mk_or(pos_or_items)));
        } else {
            connected_items.push_back(u_i == 0);
        }

        it++;
    }


    // consitent phi
    z3::expr consistent_phi = z3::mk_and(consistent_items);

    // connected_phi
    z3::expr connected_phi = z3::mk_and(connected_items);


    // sigma items
    z3::expr_vector sum_items(ctx);
    // std::set<z3::expr, exprcomp> tpaq_set;

    std::map<int, std::vector<z3::expr> >::iterator it2 = xi_to_tpaq.begin();
    while (it2 != xi_to_tpaq.end()) {
        int i = it2->first;
        std::vector<z3::expr> t_paq_vec = it2->second;
        z3::expr x_i = z3_buffer.mkIntVar("x_", i);
        z3::expr_vector s_items(ctx);
        for (int j=0; j<t_paq_vec.size(); j++) {
            s_items.push_back(t_paq_vec[j]);
        }
        sum_items.push_back(x_i == z3::sum(s_items));
        it2++;
    }


    z3::expr sum_phi = z3::mk_and(sum_items);

    std::ofstream out("consistent.smt");
    out << consistent_phi << std::endl;
    out << "\n\n\n";
    out << sum_phi << std::endl;

    out << "\nconnected_phi:\n\n";
    out << connected_phi << std::endl;


    out.close();

    z3::expr phi = consistent_phi && sum_phi && connected_phi;

    return phi;
}

void FA::print(std::string name) {
    std::cout << "write graph into " << name << std::endl;
    std::ofstream out(name);

    std::set<int> valid_ids;
    for (int i=0; i<m_accept_states.size(); i++) {
        getValidStates(m_accept_states[i], valid_ids);
    }
    // std::cout << "valid ids: " << valid_ids.size() << std::endl;

    // out vertex
    /*
      std::set<int> visited;
      std::vector<int> work_list;
      work_list.push_back(0);

      while(work_list.size() > 0) {
      int cur_state = work_list.back();
      work_list.pop_back();
      if (visited.find(cur_state) == visited.end()) {
      visited.insert(cur_state);
      // forward search
      automata::out_edge_iterator i_start, i_end;
      tie(i_start, i_end) = boost::out_edges(cur_state, m_fa);
      for(; i_start != i_end; ++i_start) {
      int i_dst = boost::target(*i_start, m_fa);
      if (visited.find(i_dst) == visited.end()) {
      work_list.push_back(i_dst);
      }
      }
      }
      }


    */
    out << "digraph g {\n";

    // out vertex
    for (int id : valid_ids) {
        out << "node_" << id << " [label=\"";
        out << vecToStr(m_fa[id]) << "("<< id <<")";
        out << "\"];\n";
    }

    for (int i=0; i<m_accept_states.size(); i++) {
        out << "node_" << m_accept_states[i] << " [shape = doublecircle];\n";
    }

    std::vector<int> work_list;
    std::set<int> visited;
    work_list.push_back(0);
    while(work_list.size() > 0) {
        int cur = work_list.back();
        work_list.pop_back();
        if (visited.find(cur) == visited.end()) {
            visited.insert(cur);
            automata::out_edge_iterator o_start, o_end;
            tie(o_start, o_end) = boost::out_edges(cur, m_fa);
            for (; o_start != o_end; ++o_start) {
                int target = boost::target(*o_start, m_fa);

                if (valid_ids.find(target) != valid_ids.end()) {
                    work_list.push_back(target);
                    out << "node_" << cur << " -> " << "node_" << target << "[label=\"";
                    std::vector<std::string> edge = m_fa[*o_start];
                    out << vecToStr(edge);

                    out << "\"];\n";
                }
            }

        }
    }


/*
// out edges
index_t index;
vertex_t v1, v2;
std::pair<edge_iter, edge_iter> ep;
edge_iter ei, ei_end;
for (tie(ei, ei_end)=edges(m_fa); ei!=ei_end; ++ei) {
v1 = boost::source(*ei, m_fa);
v2 = boost::target(*ei, m_fa);

out << "node_" << index[v1] << " -> " << "node_" << index[v2] << "[label=\"";

std::vector<std::string> edge = m_fa[*ei];

out << vecToStr(edge);

out << "\"];\n";
}
*/

    out << "}\n";


    out.close();
}

/**
 * check whether has a path from src to dst in graph by edge_count and ids
 *
 *
 */
bool FA::hasPath(int src, int dst,  std::set<int>& ids, std::map<std::string, int>& edge_count) {

    // std::cout << "find path: from: " << src << " to " << dst << std::endl;
    if (src == dst) return true;

    std::vector<int> work_list;
    work_list.push_back(src);
    std::set<int> visited;

    while(work_list.size() > 0) {
        int cur = work_list.back();
        work_list.pop_back();
        if (visited.find(cur) != visited.end()) continue;
        visited.insert(cur);

        automata::out_edge_iterator i_start, i_end;
        tie(i_start, i_end) = boost::out_edges(cur, m_fa);
        for (; i_start != i_end; ++i_start) {
            int target = boost::target(*i_start, m_fa);
            if (ids.find(target) != ids.end()) {
                std::vector<std::string> edge_info = m_fa[target];
                std::string edge_key = vecToStr(edge_info);
                int id = m_fa[boost::graph_bundle][edge_key];

                std::string t_paq_name  = "t_";
                t_paq_name.append(std::to_string(cur+1)).append("_").append(std::to_string(id)).append("_").append(std::to_string(target+1));

                if (edge_count.find(t_paq_name) == edge_count.end()) {
                    // std::cout << "IN hasPath: t_paq: " << t_paq_name << " NOT EXISTS!\n";
                    continue;
                }
                int i_edge_count = edge_count[t_paq_name];
                if (i_edge_count > 0) {
                    if (target == dst) return true;
                    work_list.push_back(target);
                }
            }
        }
    }
    return false;
}

/**
 * print model -> file_name
 * @param file_name
 * @param ids : edge_count
 * @param ids : edge_count
 * @param model : z3 model
 * @param ctx
 * @param word
 */
void FA::printModel(std::string file_name, std::set<int>& ids, std::map<std::string, int>& edge_count, z3::model& model, z3::context& ctx, std::vector<std::vector<std::string> >& word) {


    /*

      z3::expr result = ctx.bool_val(true);
      // consitent
      std::map<int, std::vector<z3::expr> > xi_to_tpaq; // x_i = \Sigma (x_paq)

      z3::expr_vector consistent_items(ctx); // consistent_items
      z3::expr_vector connected_items(ctx); // conneted_items

      std::vector<z3::expr> in_flow_vec;

      std::map<int, int> id_to_pos; // in in_flow_vec
      std::map<int, std::map<int, std::vector<z3::expr> > > id_to_neighbors;

      std::vector<int> work_list;
      int accept_state = m_accept_states[0];
      work_list.push_back(accept_state);



      std::set<int> visited;
      while(work_list.size() > 0) {
      int i = work_list.back();
      work_list.pop_back();
      if (visited.find(i) != visited.end()) {
      continue;
      }
      visited.insert(i);
      automata::in_edge_iterator i_start, i_end;
      tie(i_start, i_end) = boost::in_edges(i, m_fa);
      z3::expr_vector in_items(ctx);

      if (i==0) {
      in_items.push_back(ctx.int_val(1));
      }
      for (; i_start != i_end; ++i_start) {
      int i_src = boost::source(*i_start, m_fa);

      if (visited.find(i_src) == visited.end()) {
      work_list.push_back(i_src);
      }

      std::string key = vecToStr(m_fa[*i_start]);
      int i_edge = m_fa[boost::graph_bundle][key];

      // make var t_p_a_q
      z3::expr t_p_a_q = expr_tool::mk_int_var(ctx, "t_", i_src+1, i, i+1);
      in_items.push_back(t_p_a_q);

      // tpaq_set.insert(t_p_a_q);

      // id_to_neigbors

      }


      automata::out_edge_iterator o_start, o_end;
      tie(o_start, o_end) = boost::out_edges(i, m_fa);
      z3::expr_vector out_items(ctx);
      if (i==accept_state) {
      out_items.push_back(ctx.int_val(1));
      }
      for (; o_start != o_end; ++o_start) {
      int i_target = boost::target(*o_start, m_fa);

      // if (visited.find(i_target) != visited.end()) {
      std::string key = vecToStr(m_fa[*o_start]);
      int i_edge = m_fa[boost::graph_bundle][key];
      // make var t_p_a_q
      z3::expr t_p_a_q = expr_tool::mk_int_var(ctx, "t_", i+1, i_target, i_target+1);

      out_items.push_back(t_p_a_q);

      }

      if (in_items.size() == 0) {
      std::cout << "state: " << i << ", indegree is zero\n";
      exit(-1);
      }
      if (out_items.size() == 0) {
      std::cout << "state: " << i << ", outdegree is zero\n";
      exit(-1);
      }

      z3::expr in_item = z3::sum(in_items);
      z3::expr out_item = z3::sum(out_items);

      std::cout << "state: " << i << ", in_item: " << model.eval(in_item) << ", out_item: " << model.eval(out_item) << std::endl;
      for (int k=0; k<in_items.size(); k++) {
      std::cout << "    " << in_items[k] << ": " << model.eval(in_items[k]) << std::endl;
      }

      for (int k=0; k<out_items.size(); k++) {
      std::cout << "    " << out_items[k] << ": " << model.eval(out_items[k]) << std::endl;
      }



      id_to_pos[i] = in_flow_vec.size();
      in_flow_vec.push_back(in_item);

      z3::expr i_item = (in_item == z3::sum(out_items));
      consistent_items.push_back(i_item);

      }






      // consitent phi
      z3::expr consistent_phi = z3::mk_and(consistent_items);

      // connected_phi
      z3::expr connected_phi = z3::mk_and(connected_items);


      // sigma items
      z3::expr_vector sum_items(ctx);
      // std::set<z3::expr, exprcomp> tpaq_set;

      std::map<int, std::vector<z3::expr> >::iterator it2 = xi_to_tpaq.begin();
      while (it2 != xi_to_tpaq.end()) {
      int i = it2->first;
      std::vector<z3::expr> t_paq_vec = it2->second;
      z3::expr x_i = expr_tool::mk_int_var(ctx, "x_", i);
      z3::expr_vector s_items(ctx);
      for (int j=0; j<t_paq_vec.size(); j++) {
      s_items.push_back(t_paq_vec[j]);
      }
      sum_items.push_back(x_i == z3::sum(s_items));
      it2++;
      }


      z3::expr sum_phi = z3::mk_and(sum_items);
    */






    std::cout << "write model into file: " << file_name << std::endl;

    std::ofstream out(file_name);
    out << "digraph {\n";

    std::vector<int> work_list;
    std::set<int> state_set;
    work_list.push_back(0);



    for (int i=0; i<m_accept_states.size(); i++) {
        out << "node_" << m_accept_states[i] << " [shape = doublecircle];\n";
    }

    while(work_list.size() != 0) {
        int cur_state = work_list.back();
        // std::cout << "cur_state: " << cur_state << std::endl;
        work_list.pop_back();

        state_set.insert(cur_state);

        std::string src_name = "x_";
        src_name.append(std::to_string(cur_state));
        z3::expr x_src = ctx.int_const(src_name.c_str());

        z3::expr src_val = model.get_const_interp(x_src.decl());

        automata::out_edge_iterator i_start, i_end, i_out;
        int i_dst = -1;
        std::string key;
        bool find_path = false;

        tie(i_start, i_end) = boost::out_edges(cur_state, m_fa);
        for (; i_start != i_end; ++i_start) {
            // edge : i -> i_target
            int i_target = boost::target(*i_start, m_fa);

            if (ids.find(i_target) != ids.end()) {

                // get edge count

                std::vector<std::string> target = m_fa[i_target];
                std::string edge_key = vecToStr(target);
                int id = m_fa[boost::graph_bundle][edge_key];

                z3::expr t_paq = z3_buffer.mkIntVar("t_", cur_state+1, id, i_target+1);
                std::string t_paq_name  = t_paq.to_string();

                if (edge_count.find(t_paq_name) == edge_count.end()) {
                    // std::cout << "IN printModel: t_paq: " << t_paq_name << " NOT EXISTS!\n";
                    // exit(-1);
                    continue;

                }
                int i_edge_count = edge_count[t_paq_name];

                // std::cout << "i_edge_count: " << t_paq_name << ": " << i_edge_count << std::endl;

                if (state_set.find(i_target) == state_set.end()) {
                    if (i_edge_count > 0) {
                        if (!find_path && hasPath(i_target, cur_state, ids, edge_count)) {
                            i_out = i_start;
                            i_dst = i_target;
                            key = t_paq_name;
                            find_path = true;
                        } else {
                            if (!find_path) {
                                i_out = i_start;
                                i_dst = i_target;
                                key = t_paq_name;
                            }
                        }
                    }
                } else {
                    // i_target in state_set
                    if (i_edge_count > 0) {
                        i_out = i_start;
                        i_dst = i_target;
                        key = t_paq_name;
                        break;
                    }
                }
            }
        }


        // out
        if (i_dst != -1) {


            std::vector<std::string> i_edge = m_fa[*i_out];



            word.push_back(i_edge);
            edge_count[key]--;

            std::string edge_info = vecToStr(i_edge);
            work_list.push_back(i_dst);

            // std::cout << "select : " << i_dst << ", edge: " << edge_info << std::endl;


            out << "node_" << cur_state  << " -> " << "node_" << i_dst  << "[label=\""<< edge_info <<"\"]\n";
        }
    }

    for (int id : state_set) {
        out << "node_" << id << " [label=\"";
        out << vecToStr(m_fa[id]) << "(" << id <<")";
        out << "\"];\n";
    }


    out << "}\n";

    out.close();

}



void FA::printFlow(int accept_state) {
    std::cout << "write into flow.dot\n";
    std::ofstream out("flow.dot");

    bool is_accept = false; // check have and only have one accept state
    std::vector<int> work_list;
    work_list.push_back(accept_state);


    out << "digraph {\n";


    out << "node_" << accept_state << " [shape = doublecircle];\n";


    std::set<int> visited;
    while(work_list.size() > 0) {
        int i = work_list.back();
        work_list.pop_back();
        if (visited.find(i) != visited.end()) {
            continue;
        }
        visited.insert(i);
        automata::in_edge_iterator i_start, i_end;
        tie(i_start, i_end) = boost::in_edges(i, m_fa);

        if (i==0) {
            is_accept = true;
        }
        for (; i_start != i_end; ++i_start) {
            int i_src = boost::source(*i_start, m_fa);

            if (visited.find(i_src) == visited.end()) {
                work_list.push_back(i_src);
            }

            std::string key = vecToStr(m_fa[*i_start]);

            out << "node_" << i_src << " -> " << "node_" << i  << "[label=\""<< key <<"\"]\n";
        }
    }


    if (!is_accept) {
        // init -> accept is conneted
        std::cout << "flow is NON-CONNECTED!\n";
        exit(-1);
    }

    out << "}\n";


    out.close();

}


std::string FA::vecToStr(std::vector<std::string>& vec, std::string sep) {
    if (vec.size() == 0) return "";
    std::string result;
    result.append(vec[0]);
    for (int i=1; i<vec.size(); i++) {
        result.append(sep).append(vec[i]);
    }
    return result;
}

bool FA::isSameAlphabet(FA& other) {
    bool result = m_alphabet.size() == other.m_alphabet.size();
    for (int i=0; result && i<m_alphabet.size(); i++) {
        if (m_alphabet[i] != other.m_alphabet[i]) result = false;
    }

    return result;
}

/**
 * vec1 inter vec2
 * @param vec1 : [XX01XX]
 * @param vec2 : [XX11XX]
 * @param result: []
 * @return false, if empty
 */
bool FA::intersetEdge(std::vector<std::string> &vec1, std::vector<std::string> &vec2, std::vector<std::string> &result) {
    assert(vec1.size() == vec2.size());
    for (int i=0; i<vec1.size(); i++) {
        int case_i = 0;
        if (vec1[i] == "0") case_i += 1;
        if (vec1[i] == "1") case_i += 2;
        if (vec2[i] == "0") case_i += 3;
        if (vec2[i] == "1") case_i += 6;
        switch(case_i) {
        case 0:
            result.push_back("X");
            break;
        case 1:
        case 3:
        case 4:
            result.push_back("0");
            break;
        case 2:
        case 6:
        case 8:
            result.push_back("1");
            break;
        case 5:
        case 7:
            return false;
            break;
        default:
            std::cout << "FA PRODUCT EDGE INTERSET ERROR!\n";
            exit(-1);
        }
    }
    return true;
}
