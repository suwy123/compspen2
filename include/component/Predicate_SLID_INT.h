#if !defined(PREDICATE_SLID_INT_)
#define PREDICATE_SLID_INT_
/******************************************
*  @file  Predicate_SLID_INT.h             *
*  @brief  Brief file description          *
*                                          *
*  @author   Xincai Gu                     *
*  @version  1.0                           *
*  @date     2021-4-8                      *
*                                          *
*******************************************/


#include "component/Predicate.h"
#include "solver_slid_int/expr_tool.h"


/**
 * the recursive definition
 * binding arguments (var env)
 * base_rule, rec_rules
 *
 */
class Predicate_SLID_INT:public Predicate {
private:
	z3::func_decl m_fun;
	z3::expr m_data;
    z3::expr m_pto;
    z3::expr m_rec_app;
public:
		Predicate_SLID_INT(z3::context& ctx, z3::func_decl fun, z3::expr_vector pars, z3::expr base, z3::expr rec);
		virtual ~Predicate_SLID_INT();
		
		void check_rec_rule(z3::expr &rec_rule);
		
		z3::expr_vector& get_pars() {return m_pars;}
        void get_data_pars(z3::expr_vector& data_pars);
        
        z3::func_decl get_fun() {return m_fun;}
        z3::expr get_data() {return m_data;}
        z3::expr get_pto() {return m_pto;}
        z3::expr get_rec_app() {return m_rec_app;}
        
        void get_x_h(z3::expr_vector& x_h_vec);
        

        int size_of_static_parameters();
        int idx_E_gamma();

        //z3::expr get_plfld();

        // list with set phi_p
        bool is_listset();
        z3::expr get_phi_p();
        void get_subset_relation(std::vector<int>& sub_r);
        

        virtual void show();
};


#endif
