#ifndef LISTSOLVER_
#define LISTSOLVER_

#include "component/Problem.h"
#include "component/Predicate_SLID_INT.h"
#include "solver_slid_int/listgraph.h"
#include "solver_slid_int/expr_tool.h"

class listsolver{

protected:

        // check sat
        z3::expr get_abstraction(z3::expr& formula, z3::expr& data, z3::expr& space, z3::expr_vector& new_bools);
        z3::expr get_abstraction(z3::expr& formula, z3::expr_vector& new_bools);
        void get_data_space(z3::expr &formula, z3::expr &data, z3::expr &space);
        z3::expr abs_space(z3::expr &space, z3::expr_vector& new_bools);
        z3::expr abs_phi_star(z3::expr_vector& new_bools);

        // check entl
        int get_const_vec_and_eq_class(z3::expr& phi, z3::expr phi_abs, std::vector<z3::expr>& const_vec, std::vector<int>& const_eq_class);
        bool get_next_omega(std::vector<int>& curr, std::vector<int>& target);
        void get_omega_phi_abs(z3::expr& phi_abs, listgraph& g, std::vector<int>& omega, z3::expr& space, z3::expr& omega_phi_abs);
        bool check_allocating_plans(listgraph& g_phi, listgraph& g_psi, z3::expr& phi_abs);

        // match
        virtual bool match_graph(listgraph& psi_g, listgraph& omega_g_i);
        bool match_pto(z3::expr& psi_atom, z3::expr& omega_atom);
        bool match_path_to_atom_space(std::vector<int>& paths, z3::expr& atom_space, std::vector<z3::expr>&  oemga_const_vec, std::vector<int>&  omega_const_eq_class_vec, std::vector<int>& omega_eq_to_eq_table);
        bool match_path_to_atom_space(std::vector<int>& paths, z3::expr& atom_space);

        void unfold_by_path(std::vector<int>& path, z3::expr& psi_atom, z3::expr& oemga_f);
        void unfold_pto(Predicate_SLID_INT *pred, z3::expr& data, std::vector<z3::expr>& atom_space);
        void unfold_pred(Predicate_SLID_INT *pred, std::vector<z3::expr>& atom_space);


        // construct graph
        void construct_graph(std::vector<z3::expr>& lconsts, std::vector<int>& lconst_class, std::vector<std::set<int> >& eq_class_vec, z3::expr& space, listgraph& g);
        void get_eq_class(z3::expr& phi_abs, std::vector<z3::expr>& lconsts, std::vector<std::set<int> >& eq_class_vec,  std::vector<int>& lconst_class);
        void get_eq_class(z3::expr& phi_abs, std::vector<z3::expr>& lconsts, std::vector<std::set<int> >& eq_class_vec);
        void get_edge_from_atom(z3::expr& atom, std::vector<z3::expr>& lconsts, std::pair<std::pair<int, int>, int>& edge);



        // data_closure
        void compute_all_data_closure();
        z3::expr compute_data_closure(Predicate_SLID_INT *pred);
        z3::expr normalize_item(z3::expr x);
        z3::expr compute_alpha_closure(std::vector<std::vector<z3::expr>>& data_items, z3::expr_vector& xi, z3::expr& gamma_i, z3::expr& beta_i);

        // aux
        int get_numeral(z3::expr x);
        int index_of_vec(z3::expr x, z3::expr_vector& vec);
        void init_int_vector(std::vector<int>& vec, int size);
        int index_of_int(int x, std::vector<int>& vec);

        // output
        bool is_allocated(z3::model& m, z3::expr& source, int n);
        void write_pto(z3::model& m, z3::expr& pto, int i, int n, std::string& node_str, std::string& edge_str);
        void write_pred_pto(z3::model& m, z3::model& data_m, z3::expr& pto, int i, int n, z3::expr& plfld_interp, z3::expr& plfld, int node_i, int k, std::string& node_str, std::string& edge_str);
        z3::expr get_interp(z3::model& m, z3::expr exp);
protected:
        z3::expr pred2abs(z3::expr& atom, int i, z3::expr_vector& new_bools);
        z3::expr pred_unfold_0(z3::expr& atom, int i, z3::expr_vector& new_bools);
        virtual z3::expr pred_unfold_ge1(z3::expr& atom, int i, z3::expr_vector& new_bools);
        bool is_emp(z3::expr& abs, z3::expr& space);


public:
listsolver(Problem* problem);
//        void check_preds();
        void solve();
        z3::check_result check_sat();
        z3::check_result check_entl();

        std::string get_model_of_atom(z3::model& m, z3::expr& atom, int i, int n);
//        z3::model get_model();
        virtual ~listsolver(){}
protected:
	Problem* m_problem;
	
	z3::expr delta_ge1_predicate;
	
	z3::expr m_phi_abs;
	z3::expr m_phi_space;
	std::vector<z3::expr> m_phi_const_vec;
	std::vector<int> m_phi_const_eq_class_vec;
	
	z3::expr m_psi_abs;
	z3::expr m_psi_space;
	std::vector<z3::expr> m_psi_const_vec;
	std::vector<int> m_psi_const_eq_class_vec;
	std::vector<int> m_psi_eq_to_eq_table;
	
	int m_omega_eq_size;
};



#endif /* LISTSOLVER_ */
