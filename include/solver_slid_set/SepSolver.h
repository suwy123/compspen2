#if !defined(SEPSOLVER_)
#define SEPSOLVER_
/*******************************************
*  @file  SepSolver.h                      *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2019-2-18                     *
*                                          *
*******************************************/

#include "component/Problem.h"
#include "component/Predicate_SLID_SET.h"
#include "solver_slid_set/Graph.h"

using EdgeType = pair<pair<int, int>, int>;
using RelationMatrix = vector<vector<int>>;

class SepSolver {
public:
    SepSolver();
    virtual ~SepSolver(); 
    void setProblem(Problem* problem); 

    void solve();

    string checkSat();
    string checkEntl();
    string check(expr& phi, expr_vector& free_items);
    
    expr getAbsPhi(expr_vector& free_items);
    expr getAbsPsi(expr_vector& free_items);

    void initRm();
    void computeEqClass();

    void constructPhiGraph(Graph& g);
    void constructPsiGraph(Graph& g);

    bool checkAllocatingPlans(Graph& g_phi, Graph& g_psi);
    bool matchGraph(Graph& omega, Graph& g_psi);
    bool matchPto(expr& psi_atom, expr& omega_atom);
    bool _matchPredicate(expr& psi_atom, vector<int>& path);
    bool matchPredicate(expr& psi_atom, vector<int>& path);
    
    void showEqClass();
    void showRM();
    void printPhi(Graph& g_phi, string fname);
    
protected:
	expr getSpatialAbs(expr& atom, int i, expr_vector& new_bools, expr_vector& free_items);
    expr getSpatialStar(expr_vector& new_bools);
    expr getAbs(expr_vector& free_items, expr& phi);

    void _constructPhiGraph(Graph& g, RelationMatrix& rm);
    void _computeEqClass(RelationMatrix& rm, vector<int>& loc_eq, vector<set<int>>& eq_class);
    void atomToEdge(expr& atom, EdgeType& edge, vector<expr>& locations);

    void initInfo(expr& phi, vector<expr>& locations, RelationMatrix& rm);
    void initRm(expr& abs, vector<expr>& locations, RelationMatrix& rm, expr_vector& free_items);
    void initPhiAllocated();

    void getOmegaAbs(expr& omega_abs_i, Graph& omega_g_i, vector<int>& omega, expr& omega_abs_i1); // next omega
    bool nextOmega(vector<int>& curr, vector<int>& target);

    bool checkPsiPredEmpty(expr& psi_pred);
    
private:
    int getSuffixIdx(string& str);

protected:
    Problem* m_problem;
    
    z3::expr m_abs_phi;
    expr_vector m_phi_free_items;
    z3::expr m_abs_psi;
    expr_vector m_psi_free_items;

    vector<expr> m_phi_location;
    vector<int> m_phi_location_eq_class;
    vector<set<int>> m_phi_eq_class;
    RelationMatrix m_phi_location_relation;
    vector<int> m_phi_space_allocated;

    vector<expr> m_psi_location;
    vector<int> m_psi_location_eq_class;
    vector<set<int>> m_psi_eq_class;
    RelationMatrix m_psi_location_relation;
    
    expr_vector m_new_vars;
    
    int m_counter;
};

#endif
