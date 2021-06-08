#include "component/Predicate_SLID_INT.h"

//extern z3::context z3_ctx;

Predicate_SLID_INT::Predicate_SLID_INT(z3::context& ctx, z3::func_decl fun, z3::expr_vector pars, z3::expr base, z3::expr rec)
        :Predicate(ctx, pars,base,rec), 
        m_fun(fun), m_data(ctx), m_pto(ctx), m_rec_app(ctx) {
        	check_rec_rule(rec);
		}
		
Predicate_SLID_INT::~Predicate_SLID_INT() {
}

void Predicate_SLID_INT::check_rec_rule(z3::expr &rec_rule) {
        // pars
        if (!rec_rule.is_quantifier()) {
                cout<<"'exists' is excepted in the recursive rule"<<endl;
        }
        z3::expr body = rec_rule.body();
        if (!body.is_app()) {
                cout<<"app is excepted in the recursive rule"<<endl;
        }
        if (body.decl().name().str() != "and") {
            z3::expr_vector and_items(z3_ctx);
            and_items.push_back(body);
            body = z3::mk_and(and_items);
        }
        // 1. data
        z3::expr_vector data_items(z3_ctx);
		int num = body.num_args();
        for (int i=0; i<num-1; i++) {
        	data_items.push_back(body.arg(i));
    	}
    	m_data = mk_and(data_items);


        z3::expr sep = body.arg(num-1);
        if (sep.arg(0).decl().name().str() != "pto") {
                cout<<"'pto' is excepted first in the 'sep'"<<endl;
        }
        // 2. pto
        m_pto = sep.arg(0);
        // 3. rec app
        m_rec_app = sep.arg(1);
}

void Predicate_SLID_INT::get_x_h(z3::expr_vector& x_h_vec) {
        std::set<z3::expr, exprcomp> x_h;
        expr_tool::get_vars(m_data, x_h);
        expr_tool::get_vars(m_pto, x_h);
        expr_tool::get_vars(m_rec_app, x_h);
        for (auto exp : x_h) {
                x_h_vec.push_back(exp);
        }
}

void Predicate_SLID_INT::show() {
    cout << "pars: \n";
    for (unsigned int i=0; i<m_pars.size(); i++) {
        cout << m_pars[i] << " ";
    }
    cout << endl;

    cout << "base: " << m_base_rule << endl;
    cout << "rec: " << m_rec_rule << endl;

    cout << "pto: " << m_pto <<endl;
    cout << "recursive app: " << m_rec_app <<endl;
    cout << "data formula: " << m_data << endl;

}

/**
 * the size of static parameters for list predicate
 * @return size
 */
int Predicate_SLID_INT::size_of_static_parameters() {
        //if (is_list()) {
            int i = m_pars.size()-1;
            for (; i>=0; i--) {
                    if (m_pars[i].to_string().find("sta_")!=0) break;
            }
            return m_pars.size() - i - 1;
        //}
        //return 0;
}

void Predicate_SLID_INT::get_data_pars(z3::expr_vector& data_pars) {
    for (int i=0; i<m_pars.size(); i++) {
        if (!expr_tool::is_location(m_pars[i])) {
            data_pars.push_back(m_pars[i]);
        }
    }
}

/**
 * the index, when E occurs in gamma for list predicate
 * @return -1, E does not occur in gamma
 *         index
 */
int Predicate_SLID_INT::idx_E_gamma() {
        //if (is_list()) {
                int size = m_pars.size() - size_of_static_parameters();
                for (int i=1; i<size/2; i++) {
                        if (m_rec_app.arg(i).hash() == m_pars[0].hash()) return i-1;
                }
        //}
        return -1;
}

/**
 * get principal location filed
 */
//z3::expr Predicate_SLID_INT::get_plfld() {
//        if (is_list()) {
//                z3::expr rec_app_source = m_rec_app.arg(0);
//                z3::expr sref = m_pto.arg(1);
//                if (sref.decl().name().str() == "sref") {//(sref (ref next ?u) (ref data ?d))
//                        for (int j=0; j<sref.num_args(); j++) {
//                                z3::expr ref = sref.arg(j);
//                                z3::expr dest = ref.arg(1);
//                                if (dest.hash() == rec_app_source.hash()) {
//                                        return ref.arg(0);
//                                }
//                        }
//                } else {
//                        return sref.arg(0);//(ref next ?u)
//
//                }
//        }
//        return m_pars[0];
//}

/**
 * get phi_p (delta) of list in inductive rule
 */
z3::expr Predicate_SLID_INT::get_phi_p() {
        z3::expr delta = m_data;
        int st_size = size_of_static_parameters();
        int size = m_pars.size() - st_size;
        z3::expr rec_app = m_rec_app;

        z3::expr_vector src_pars(z3_ctx);
        z3::expr_vector dst_pars(z3_ctx);
        for (int i=0; i<size/2; i++) {
                src_pars.push_back(rec_app.arg(i));
                dst_pars.push_back(rec_app.arg(i+size/2));
        }
        delta = delta.substitute(src_pars, dst_pars); // delta = phi_r1 && phi_r2
        return delta;
}

bool Predicate_SLID_INT::is_listset() {
        for (int i=0; i<m_pars.size()-1; i++) {
                if (expr_tool::is_setint(m_pars[i])) return true;
        }
        return false;
}

/**
 * list with set: get subset relation of alpha set with beta set
 * @param sub_r:  [alpha_i sub beta_i -> 1] alpha_i sub beta_i
 */
void Predicate_SLID_INT::get_subset_relation(std::vector<int> &sub_r) {
        if (is_listset()) {
                z3::expr delta = m_data;
                int st_size = size_of_static_parameters();
                int size = m_pars.size() - st_size;

                std::vector<z3::expr> alpha;
                for (int i=1; i<size/2; i++) {
                        alpha.push_back(m_pars[i]);
                        sub_r.push_back(-1);
                }
                for (int i=0; i<delta.num_args(); i++) {
                        z3::expr item_i = delta.arg(i);
                        if (item_i.num_args() == 2) {
                                z3::expr s1 = item_i.arg(0);
                                int idx = -1;
                                int sub = 0;
                                if (expr_tool::is_setint(s1) && s1.is_var()) {
                                        z3::expr s2 = item_i.arg(1);
                                        if (s2.num_args() == 2) {
                                                s2 = s2.arg(0);
                                        }
                                        idx = expr_tool::index_of_exp(s2, alpha);
                                        sub = 1;
                                } else {
                                        idx = expr_tool::index_of_exp(s1, alpha);
                                }
                                if (idx != -1) {
                                        sub_r[idx] = sub;
                                }
                        }
                }
        }
}
