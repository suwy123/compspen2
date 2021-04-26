#include <queue>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <fstream>

#include "solver_slid_int/listgraph.h"

// using namespace std;

/* --------------------------------------------------------------------------*/
/**
 * @synopsis  graph initialize a graph using abstraction
 *
 * @param f separation logic formula
 */
/* --------------------------------------------------------------------------*/

listgraph::listgraph(const listgraph& g)
{
	cc = g.cc;
	cycle = g.cycle;
	edge_cycle = g.edge_cycle;
	adj_list = g.adj_list;
}
listgraph& listgraph::operator=(const listgraph& g)
{
	if (this == &g)
		return *this;
	cc = g.cc;
	cycle = g.cycle;
	edge_cycle = g.edge_cycle;
	adj_list = g.adj_list;
}
listgraph::listgraph(listgraph&& g) noexcept
{
	cc = g.cc;
	cycle = g.cycle;
	edge_cycle = g.edge_cycle;
	adj_list = g.adj_list;
}
listgraph& listgraph::operator=(listgraph&& g) noexcept
{
	if (this == &g)
		return *this;
	cc = g.cc;
	cycle = g.cycle;
	edge_cycle = g.edge_cycle;
	adj_list = g.adj_list;
}
void listgraph::init(std::vector<std::set<int> >& eq_class_vec,  std::vector<std::pair<std::pair<int, int>, int> >& edge_vec)
{
	adj_list.clear();
	std::set<int> eq_class;
	/*create vertex*/
	for (size_t i = 0; i < eq_class_vec.size(); ++i) {
		eq_class = eq_class_vec[i];
		// add one vertex of index i
		boost::add_vertex(adj_list);
		// initialize the property of the ith vertex
		adj_list[i].insert(begin(eq_class), end(eq_class));
		// initialize the property of graph
		for (auto it = begin(eq_class); it != end(eq_class); ++it)
			adj_list[boost::graph_bundle][*it] = i;
	}


	/*create edge*/
	std::pair<std::pair<int, int>, int> edge;
	int src, dst;
	for (int i=0; i<edge_vec.size(); i++) {
		edge = edge_vec[i];
		src = adj_list[boost::graph_bundle][edge.first.first];
		dst = adj_list[boost::graph_bundle][edge.first.second];
		boost::add_edge(src, dst, edge.second, adj_list);
	}

	seek_cc();
	seek_cycle();
}



void listgraph::print(std::vector<z3::expr>& lconsts, z3::expr& space, std::string file_name) {
	std::cout << "print graph ----> " << file_name <<std::endl;
	std::ofstream out(file_name);

	boost::property_map<adjacency_list, boost::vertex_index_t>::type index;
	std::pair<vertex_iterator, vertex_iterator> vp;
	adjacency_list::vertex_descriptor v;
	// size_t num = boost::num_vertices(adj_list);
	out << "digraph g {\n";
	for (vp=vertices(adj_list); vp.first!=vp.second; ++vp.first) {
		v = *vp.first;
		std::set<int> node = adj_list[v];
		out << "node_" << index[v] << " [label=\"[";
		std::set<int>::iterator it=node.begin();
		if (it != node.end()){
			out << lconsts[*it];
			++it;
		}

		for( ;it!=node.end(); ++it) {
			out <<", " << lconsts[*it];
		}
		out <<"]\"];\n\n";
	}

	adjacency_list::vertex_descriptor  v1, v2;
	std::pair<edge_iterator, edge_iterator> ep;
	edge_iterator ei, ei_end;
	for (tie(ei, ei_end)=edges(adj_list); ei!=ei_end; ++ei) {
		v1 = boost::source(*ei, adj_list);
		v2 = boost::target(*ei, adj_list);

		out << "node_" << index[v1] << "->" << "node_" << index[v2] << "[label=\"";
		out << space.arg(adj_list[*ei]);
		out<<"\"];\n";
	}
	out << "}\n";
	out.close();
}


void listgraph::get_edges(std::vector<std::pair<std::pair<int, int>, int> >& edge_vec) {
	adjacency_list::vertex_descriptor  v1, v2;
	std::pair<edge_iterator, edge_iterator> ep;
	edge_iterator ei, ei_end;
	for (tie(ei, ei_end)=edges(adj_list); ei!=ei_end; ++ei) {
		v1 = boost::source(*ei, adj_list);
		v2 = boost::target(*ei, adj_list);
		std::pair<std::pair<int, int>, int> edge;
		std::set<int> v1_set = adj_list[v1];
		edge.first.first = *(v1_set.begin());
		std::set<int> v2_set = adj_list[v2];
		edge.first.second = *(v2_set.begin());
		edge.second = adj_list[*ei];
		edge_vec.push_back(edge);
	}
}


void listgraph::print() {
	std::cout << "print graph:\n";
	std::ofstream out("graph.dot");

	boost::property_map<adjacency_list, boost::vertex_index_t>::type index;
	std::pair<vertex_iterator, vertex_iterator> vp;
	adjacency_list::vertex_descriptor v;
	size_t num = boost::num_vertices(adj_list);
	std::cout << "num of vertices: " << num << std::endl;
	out << "digraph g {\n";
	for (vp=vertices(adj_list); vp.first!=vp.second; ++vp.first) {
		v = *vp.first;
		std::cout <<"size: " << adj_list[v].size() << " , index: " << index[v]  << std::endl;
		std::set<int> node = adj_list[v];
		out << "node_" << index[v] << " [label=\"[";
		std::set<int>::iterator it=node.begin();
		if (it != node.end()){
			out << *it;
			++it;
		}

		for( ;it!=node.end(); ++it) {
			out <<", " << *it;
		}
		out <<"]\"];\n\n";
	}
	std::cout << std::endl;



	adjacency_list::vertex_descriptor  v1, v2;
	std::pair<edge_iterator, edge_iterator> ep;
	edge_iterator ei, ei_end;
	for (tie(ei, ei_end)=edges(adj_list); ei!=ei_end; ++ei) {
		v1 = boost::source(*ei, adj_list);
		v2 = boost::target(*ei, adj_list);
		std::cout << "(" << *adj_list[v1].begin() << ") ---";
		std::cout << adj_list[*ei] ;
		std::cout << "--- (" << *adj_list[v2].begin() << ")";
		std::cout<< std::endl;

		out << "node_" << index[v1] << "->" << "node_" << index[v2] << "[label=\"";
		out << adj_list[*ei];
		out<<"\"];\n";
	}
	std::cout << std::endl;
	out << "}\n";
	out.close();
}

/* --------------------------------------------------------------------------*/
/**
 * @synopsis  get_edge_property
 *
 * @param u   vertex index u
 * @param v
 *
 * @return    edge property representing spatial atom id
 *		  -1 no edge between u and v
 */
/* --------------------------------------------------------------------------*/
int listgraph::get_edge_property(size_t u, size_t v) const
{
	adjacency_list::out_edge_iterator it1, it2;
	tie(it1, it2) = boost::out_edges(u, adj_list);
	for (; it1 != it2; ++it1) {
		if (v == boost::target(*it1, adj_list))
			return adj_list[*it1];
	}
	return -1;
}
int listgraph::get_edge_property(edge_descriptor e)
{
	return get_edge_property(source(e), target(e));
}
/* --------------------------------------------------------------------------*/
/**
 * @synopsis  seek_cc calculate the connected components
 */
/* --------------------------------------------------------------------------*/
void listgraph::seek_cc()
{
	if (cc.size() > 0) cc.clear();
	size_t i, j, k;
	adjacency_list::out_edge_iterator out_it1, out_it2;
	adjacency_list::in_edge_iterator in_it1, in_it2;
	std::deque<int> q;
	std::set<int> t;
	size_t num = boost::num_vertices(adj_list);
	std::vector<int> visited(num);

	for (i = 0; i < visited.size(); ++i)
		visited[i] = 0;

	for (i = 0; i < num; ++i) {
		if (visited[i])
			continue;
		q.push_back(i);
		t.clear();
		while (!q.empty()) {
			j = q.front();
			visited[j] = 1;
			t.insert(j);
			tie(in_it1, in_it2) = boost::in_edges(j, adj_list);
			for (; in_it1 != in_it2; ++in_it1) {
				k = boost::source(*in_it1, adj_list);
				if (!(visited[k]) && (find(begin(q), end(q), k) == end(q)))
					q.push_back(k);
			}
			tie(out_it1, out_it2) = boost::out_edges(j, adj_list);
			for (; out_it1 != out_it2; ++out_it1) {
				k = boost::target(*out_it1, adj_list);
				if (!(visited[k]) && (find(begin(q), end(q), k) == end(q)))
					q.push_back(k);
			}
			q.pop_front();
		}
		cc.push_back(t);
	}
}
/* --------------------------------------------------------------------------*/
/**
 * @synopsis  seek_cycle calculate the cycle of the graph
 */
/* --------------------------------------------------------------------------*/
void listgraph::seek_cycle()
{
	if (cycle.size() > 0) cycle.clear();
	if (edge_cycle.size() > 0) edge_cycle.clear();
	size_t i, j;
	std::vector<std::vector<int>> t;
	edge_cycles_t e_cycles;
	adjacency_list::out_edge_iterator it1, it2;
	std::vector<int> c, t1, s;
	std::map<int, bool> m;
	for (i = 0; i < cc.size(); ++i) { // 连通分支
		c.assign(begin(cc[i]), end(cc[i])); // grpah node

		// cc [1,2,3,...]
		for (j = 0; j < c.size(); ++j)
			m[c[j]] = false;
		t.clear();
		e_cycles.clear();
		for (j = 0; j < c.size(); ++j) {
			if (m[c[j]])
				continue;
			s.push_back(c[j]); // s [1->x->y->...]
			while (!s.empty()) {
				t1.clear();
				size_t k = s.back();
				if (!m[k]) {
					tie(it1, it2) = boost::out_edges(k, adj_list);
					for (; it1 != it2; ++it1) {
						size_t tar = boost::target(*it1, adj_list);
						auto it = find(begin(s), end(s), tar);
						if (it != end(s)) {
							t1.insert(end(t1), it, end(s)); // it->...->end(s)->it
							t.push_back(t1);
							// add edge_cycle
							edge_cycle_t e_cycle;
							for (int idx=0; idx+1<t1.size(); idx++) {
								edge_t edge;
								edge.first.first = t1[idx];
								edge.first.second = t1[(idx+1)];
								edge.second = get_edge_property(t1[idx], t1[(idx+1)]);
								e_cycle.push_back(edge);
							}
							std::pair<int,int> eg(k, tar);
							edge_t edge(eg, adj_list[*it1]);
							e_cycle.push_back(edge);
							e_cycles.push_back(e_cycle);


							t1.clear();
						}
					}
					m[k] = true;
				}
				tie(it1, it2) = boost::out_edges(k, adj_list);
				for (; it1 != it2; ++it1) {
					size_t tar = boost::target(*it1, adj_list);
					if(!m[tar]) {
						s.push_back(tar);
						break;
					}
				}
				if (it1 == it2){
					s.pop_back();
				}
			}
		}

		cycle.push_back(t);
		edge_cycle.push_back(e_cycles);

	}
}

/* --------------------------------------------------------------------------*/
/**
 * @synopsis  is_dag_like
 * @return
 */
/* --------------------------------------------------------------------------*/
bool listgraph::is_dag_like() const
{
	assert(!cycle.empty());
	size_t i, j, k;
	adjacency_list::out_edge_iterator it1, it2;
	std::set<int> v_out_cycle;//

	for (i = 0; i < cycle.size(); ++i) {
		if (cycle[i].size() > 1)
			return false;

		if (cycle[i].size() == 0) continue;
		// check dag-like
		v_out_cycle.clear();
		v_out_cycle.insert(begin(cc[i]), end(cc[i]));

		for (k = 0; k < cycle[i][0].size(); ++k) {
			v_out_cycle.erase(cycle[i][0][k]);
		}
		// v_out_cycle -> cycle reachability
		for (auto v : v_out_cycle) {
			bool flag = false;
			for (auto u : cycle[i][0]) {
				std::vector<listgraph::edge_descriptor> res = get_path(v, u);
				if (!res.empty()) {flag=true;break;}
			}
			if (!flag) return false;
		}
	}
	return true;
}

std::vector<int> listgraph::get_cc_cycle_num() const
{
	std::vector<int> res(cc.size());

	for (size_t i = 0; i < cycle.size(); ++i)
		res[i] = cycle[i].size();

	return res;
}

std::vector<listgraph::cc_cycle_t>& listgraph::get_cc_cycle()
{
	return cycle;
}

int listgraph::which_cc(size_t v) const
{
	for (size_t i = 0; i < cc.size(); ++i) {
		auto it = std::find(begin(cc[i]), end(cc[i]), v);
		if (it != end(cc[i]))
			return i;
	}
	return -1;
}

std::pair<int, int> listgraph::get_cycle_coord(size_t v) const
{
	int i = which_cc(v);
	int j;
	for (size_t k = 0; k < cycle[i].size(); ++k) {
		auto it = std::find(begin(cycle[i][k]), end(cycle[i][k]), v);
		if (it != end(cycle[i][k]))
			return std::make_pair(i, k);
	}
	return std::make_pair(-1, -1);
}

std::pair<int, int> listgraph::get_cycle_coord(edge_descriptor e) const
{
	return get_cycle_coord(boost::source(e, adj_list));
}

bool listgraph::is_cycle(const std::pair<int, int>& coord) const
{
	return (coord.first >= 0 && coord.second >= 0);
}

std::vector<int> listgraph::get_cycle(const std::pair<int, int>& coord) const
{
    assert(coord.first < cycle.size() && coord.second < cycle[coord.first].size());
	return cycle[coord.first][coord.second];
}

std::vector<listgraph::edge_t> listgraph::get_edge_cycle(const std::pair<int, int>& coord) const {
    assert(coord.first < edge_cycle.size() && coord.second < edge_cycle[coord.first].size());
	return edge_cycle[coord.first][coord.second];
}

std::vector<listgraph::edge_descriptor> listgraph::merge_path(std::vector<listgraph::edge_descriptor>& path1, std::vector<listgraph::edge_descriptor>& path2)
{
	std::vector<edge_descriptor> res(path1);
	res.insert(end(res), begin(path2), end(path2));
	return res;
}

std::vector<listgraph::edge_descriptor> listgraph::merge_path(std::vector<listgraph::edge_descriptor>& path1, std::vector<int>& c) const
{
	std::vector<edge_descriptor> res(path1);
	edge_descriptor e = path1[path1.size()-1];
	out_edge_iterator ei, ei_end;
	std::vector<int>::iterator it, it2;
	size_t v = boost::target(e, adj_list);
	it = std::find(begin(c), end(c), v);
	for (it2 = it; it2 != end(c); ++it2) {
		tie(ei, ei_end) = boost::out_edges(*it2, adj_list);
		res.push_back(*ei);
	}
	for (it2 = begin(c); it2 != it; ++it2) {
		tie(ei, ei_end) = boost::out_edges(*it2, adj_list);
		res.push_back(*ei);
	}
	return res;
}

std::vector<std::pair<int, int>> listgraph::get_cycle_coords() const
{
	std::vector<std::pair<int, int>> res;
	for (size_t i = 0; i < cycle.size(); ++i) {
		for (size_t j = 0; j < cycle[i].size(); ++j)
			res.push_back(std::make_pair(i, j));
	}
	return res;
}
std::vector<listgraph::edge_descriptor> listgraph::get_path(size_t u)
{
	std::vector<edge_descriptor> res;
	std::pair<int, int> coord = get_cycle_coord(u);
	if (!is_cycle(coord))
		return res;

	std::vector<int> c = get_cycle(coord);
	out_edge_iterator ei, ei_end;
	std::vector<int>::iterator it, it2;
	it = std::find(begin(c), end(c), u);
	for (it2 = it; it2 != end(c); ++it2) {
		tie(ei, ei_end) = boost::out_edges(*it2, adj_list);
		res.push_back(*ei);
	}
	for (it2 = begin(c); it2 != it; ++it2) {
		tie(ei, ei_end) = boost::out_edges(*it2, adj_list);
		res.push_back(*ei);
	}
	return res;
}

std::set<size_t> listgraph::get_reachable_set(size_t u) {
    std::vector<edge_descriptor> res;

    std::vector<size_t> work_list;
    std::set<size_t> visited;
    work_list.push_back(u);

    while(work_list.size() > 0) {
        int cur = work_list.back();
        work_list.pop_back();
        if (visited.find(cur) != visited.end()) {
            continue;
        }
        visited.insert(cur);
        in_edge_iterator ei, ei_end;
        tie(ei, ei_end) = boost::in_edges(cur, adj_list);
        for (; ei != ei_end; ++ei) {
            size_t src = boost::source(*ei, adj_list);
            res.push_back(*ei);
            if (visited.find(src) == visited.end()) {
                work_list.push_back(src);
            }
        }
    }
    return visited;
}

std::vector<listgraph::edge_descriptor> listgraph::get_reachable_edges(size_t u) {
    std::vector<edge_descriptor> res;

    std::vector<size_t> work_list;
    std::set<size_t> visited;
    work_list.push_back(u);

    while(work_list.size() > 0) {
        int cur = work_list.back();
        work_list.pop_back();
        if (visited.find(cur) != visited.end()) {
            continue;
        }
        visited.insert(cur);
        out_edge_iterator ei, ei_end;
        tie(ei, ei_end) = boost::out_edges(cur, adj_list);
        for (; ei != ei_end; ++ei) {
            size_t targ = boost::target(*ei, adj_list);
            res.push_back(*ei);
            if (visited.find(targ) == visited.end()) {
                work_list.push_back(targ);
            }
        }
    }
    return res;
}

std::vector<listgraph::edge_descriptor> listgraph::get_path(size_t u, size_t v) const
{
	std::vector<edge_descriptor> r;
	if (u == v) return r;

	int i = which_cc(u);
	if (i != which_cc(v))
		return r;

	std::vector<int> t;
	std::vector<edge_descriptor> s;
	t.assign(begin(cc[i]), end(cc[i]));
	std::vector<int> visited(t.size());
	std::map<int, int> m;
	for (size_t i = 0; i < t.size(); ++i)
		m[t[i]] = i;
	return get_path(u, v, s, visited, m);
}

std::vector<listgraph::edge_descriptor> listgraph::get_path(size_t u, size_t v,
													std::vector<edge_descriptor>& s, std::vector<int>& visited,
													std::map<int, int>& m) const
{
	if (u == v) return s;
	std::vector<edge_descriptor> res;
	visited[m[u]] = 1;
	out_edge_iterator ei, ei_end;
	tie(ei, ei_end) = boost::out_edges(u, adj_list);
	for (; ei != ei_end; ++ei) {
		size_t targ = boost::target(*ei, adj_list);
		if (!visited[m[targ]]) {
			s.push_back(*ei);
			res = get_path(targ, v, s, visited, m);
			if (!res.empty())
				break;
			s.pop_back();
		}
	}
	return res;
}

std::pair<listgraph::edge_iterator, listgraph::edge_iterator> listgraph::get_edge()
{
	return boost::edges(adj_list);
}

size_t listgraph::get_vertex_id(size_t vid)
{
	return adj_list[boost::graph_bundle][vid];
}
size_t listgraph::source(edge_descriptor e)
{
	return boost::source(e, adj_list);
}
size_t listgraph::target(edge_descriptor e)
{
	return boost::target(e, adj_list);
}
