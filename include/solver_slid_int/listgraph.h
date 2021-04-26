#ifndef LISTGRAPH_
#define LISTGRAPH_

#include <vector>
#include <set>
#include <map>
#include <utility>
#include <z3++.h>

#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>


class listgraph {
public:
	typedef std::set<int> vertex_label;
	typedef size_t edge_label;
	typedef std::map<int, int> graph_label;

	typedef boost::adjacency_list<boost::vecS,
				      boost::vecS,
				      boost::bidirectionalS,
				      vertex_label,
				      edge_label,
				      graph_label> adjacency_list;

	typedef std::set<int> cc_t;
	typedef std::vector<int> cycle_t;
	typedef std::vector<cycle_t> cc_cycle_t;
	typedef std::pair<std::pair<int, int>, int> edge_t;
	typedef std::vector<edge_t> edge_cycle_t;
	typedef std::vector<edge_cycle_t> edge_cycles_t;
	typedef boost::graph_traits<adjacency_list>::edge_descriptor edge_descriptor;
	typedef boost::graph_traits<adjacency_list>::edge_iterator edge_iterator;
	typedef adjacency_list::out_edge_iterator out_edge_iterator;
	typedef adjacency_list::in_edge_iterator in_edge_iterator;

	typedef boost::graph_traits<adjacency_list>::vertex_iterator vertex_iterator;

	listgraph() = default;
	listgraph(const listgraph&);
	listgraph& operator=(const listgraph&);

	listgraph(listgraph&&) noexcept;
	listgraph& operator=(listgraph&&) noexcept;
	// void init(sl_abstr&);
	void init(std::vector<std::set<int> >& eq_class_vec,  std::vector<std::pair<std::pair<int, int>, int> >&);

	std::pair<edge_iterator, edge_iterator> get_edge();
	std::vector<edge_descriptor> get_path(size_t, size_t) const;
	std::vector<edge_descriptor> get_reachable_edges(size_t);
	std::vector<edge_descriptor> get_path(size_t);

	std::set<size_t> get_reachable_set(size_t);


	std::vector<int> get_cc_cycle_num() const;
	std::vector<cc_cycle_t>& get_cc_cycle();
	int which_cc(size_t) const;
	bool is_dag_like() const;


	/* only act right in dag like graph */
	std::pair<int, int> get_cycle_coord(size_t) const;
	std::pair<int, int> get_cycle_coord(edge_descriptor) const;

	bool is_cycle(const std::pair<int, int>&) const;
	std::vector<int> get_cycle(const std::pair<int, int>&) const;
	std::vector<edge_t> get_edge_cycle(const std::pair<int, int>&) const;
	std::vector<edge_descriptor> get_cycle_edge(std::pair<int, int>&);
	std::vector<edge_descriptor> merge_path(std::vector<edge_descriptor>&, std::vector<int>&) const;
	std::vector<edge_descriptor> merge_path(std::vector<edge_descriptor>&, std::vector<edge_descriptor>&);
	std::vector<std::pair<int, int>> get_cycle_coords() const;

	void seek_cc();
	void seek_cycle();

public:
	void print();
	void print(std::vector<z3::expr>& lconsts, z3::expr& space, std::string file_name="graph.dot");
	void get_edges(std::vector<std::pair<std::pair<int, int>, int> >& edge_vec);
	size_t get_vertex_id(size_t);
	size_t source(edge_descriptor);
	size_t target(edge_descriptor);
	int get_edge_property(size_t, size_t) const;
	int get_edge_property(edge_descriptor);



private:
	std::vector<edge_descriptor> get_path(size_t, size_t, std::vector<edge_descriptor>&,
						std::vector<int>&, std::map<int, int>&) const;

	std::vector<cc_t> cc;
	std::vector<cc_cycle_t> cycle;
	std::vector<edge_cycles_t> edge_cycle;

public:
	adjacency_list adj_list;

};
#endif // listgraph.h
