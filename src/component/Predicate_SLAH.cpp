#include "component/Predicate_SLAH.h"
#include "exception/SemanticException.h"

//extern z3::context z3_ctx;

Predicate_SLAH::Predicate_SLAH(z3::context& ctx, z3::func_decl fun, z3::expr_vector pars, z3::expr base, z3::expr rec)
:Predicate(ctx, pars,base,rec), m_fun(fun), m_rec_hck(z3_ctx), m_rec_app(z3_ctx){
    m_ex_size = Z3_get_quantifier_num_bound(z3_ctx, Z3_ast(rec));
    m_z_size = m_ex_size - 1;
    z3::expr body = rec.body();

    // 1. hck
    m_rec_hck = body.arg(0);
    // 2. rec app
    m_rec_app = body.arg(1);
    
    m_v_size = m_rec_hck.num_args() - 2 * m_ex_size;
}

Predicate_SLAH::~Predicate_SLAH() {
}

void Predicate_SLAH::show() {
		std::cout << "name: " << get_name() << std::endl;
        std::cout << "pars: \n" << m_pars << std::endl;
        std::cout << "base rule: \n" << m_base_rule << std::endl;
        std::cout << "rec_rule: \n"<< m_rec_hck <<" * "<<m_rec_app<<std::endl;
}
