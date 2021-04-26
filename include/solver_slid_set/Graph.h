#ifndef SL_GRAPH_H
#define SL_GRAPH_H

#include "Types.h"
#include <utility>
#include <z3++.h>

#include <boost/config.hpp>
#include <boost/utility.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>


class Graph {
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

    Graph() = default;
    Graph(const Graph&);
    Graph& operator=(const Graph&);

    Graph(Graph&&) noexcept;
    Graph& operator=(Graph&&) noexcept;
    // void init(sl_abstr&);
    void init(std::vector<std::set<int> >& eq_class_vec,  std::vector<std::pair<std::pair<int, int>, int> >&);

    std::pair<edge_iterator, edge_iterator> getEdge();
    std::vector<edge_descriptor> getPath(size_t, size_t) const;
    std::vector<edge_descriptor> getReachableEdges(size_t);
    std::vector<edge_descriptor> getPath(size_t);

    std::set<size_t> getReachableSet(size_t);


    std::vector<int> getCcCycleNum() const;
    std::vector<cc_cycle_t>& getCcCycle();
    int whichCc(size_t) const;
    bool isDagLike() const;


    /* only act right in dag like Graph */
    std::pair<int, int> getCycleCoord(size_t) const;
    std::pair<int, int> getCycleCoord(edge_descriptor) const;

    bool isCycle(const std::pair<int, int>&) const;
    std::vector<int> getCycle(const std::pair<int, int>&) const;
    std::vector<edge_t> getEdgeCycle(const std::pair<int, int>&) const;
    std::vector<edge_descriptor> getCycleEdge(std::pair<int, int>&);
    std::vector<edge_descriptor> mergePath(std::vector<edge_descriptor>&, std::vector<int>&) const;
    std::vector<edge_descriptor> mergePath(std::vector<edge_descriptor>&, std::vector<edge_descriptor>&);
    std::vector<std::pair<int, int>> getCycleCoords() const;

    void seekCc();
    void seekCycle();

public:
    void print();
    void print(std::vector<z3::expr>& lconsts, z3::expr& space, std::string file_name="Graph.dot");
    void getEdges(std::vector<std::pair<std::pair<int, int>, int> >& edge_vec);
    size_t getVertexId(size_t);
    size_t source(edge_descriptor);
    size_t target(edge_descriptor);
    int getEdgeProperty(size_t, size_t) const;
    int getEdgeProperty(edge_descriptor);


private:
    std::vector<edge_descriptor> getPath(size_t, size_t, std::vector<edge_descriptor>&,
                        std::vector<int>&, std::map<int, int>&) const;

    std::vector<cc_t> cc;
    std::vector<cc_cycle_t> cycle;
    std::vector<edge_cycles_t> edge_cycle;

public:
    adjacency_list adj_list;

};
#endif // 
