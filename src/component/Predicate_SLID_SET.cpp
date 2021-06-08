#include "component/Predicate_SLID_SET.h"
//#include "component/Z3Buffer.h"
#include "Types.h"


//extern z3::context z3_ctx;
//extern Z3Buffer z3_buffer;

Predicate_SLID_SET::Predicate_SLID_SET(z3::context& ctx, Z3Buffer& buffer, z3::expr_vector pars, z3::expr base, z3::expr rec)
        :Predicate(ctx, pars,base,rec),z3_buffer(buffer),  
        m_data(ctx), m_pto(ctx), m_rec_app(ctx), 
        m_deltap(ctx), m_succ(ctx), m_succ_pars(ctx),
        m_case_i(-1), m_svars(ctx), m_strt_items(ctx), m_strt_pars(ctx),
        m_tr(ctx), m_free_item(ctx) {
    expr_vector x(z3_ctx);
    getX(x);
    expr body = m_rec_rule.body().substitute(x);  

    int num = body.num_args();
    expr_vector data_items(z3_ctx);
    for (int i=0; i<num-1; i++) {
        data_items.push_back(body.arg(i));
    }
    m_data = mk_and(data_items);

    expr sep_app = body.arg(num-1);
    m_pto = sep_app.arg(0);
    m_rec_app = sep_app.arg(1);

    m_deltap = getDeltaP();

    initSucc();
    m_tr = getTr();
}


Predicate_SLID_SET::~Predicate_SLID_SET() {
}

expr Predicate_SLID_SET::unfoldPredicate(expr_vector& args) {
    expr_vector x(z3_ctx);
    for (int i=args.size()-1; i>=0; i--) {
        x.push_back(args[i]);
    }
    expr body = m_rec_rule.body().substitute(x);

    int num = body.num_args();
    expr_vector data_items(z3_ctx);
    for (int i=0; i<num-1; i++) {
        data_items.push_back(body.arg(i));
    }
    expr_vector and_items(z3_ctx);
    and_items.push_back(mk_and(data_items));

    and_items.push_back(body.arg(num-1).arg(0));
    and_items.push_back(body.arg(num-1).arg(1));
    return mk_and(and_items);
}

void Predicate_SLID_SET::getABC(expr_vector& alpha, expr_vector& beta, expr_vector& gamma) {

}

void Predicate_SLID_SET::getAlpha(expr_vector& alpha) {
    for (unsigned int i=1; i<m_pars.size()/2; i++) {
        alpha.push_back(m_pars[i]);
    }
}

void Predicate_SLID_SET::getBeta(expr_vector& beta) {
    for (unsigned int i=m_pars.size()/2+1; i<m_pars.size(); i++) {
        beta.push_back(m_pars[i]);
    }
}

void Predicate_SLID_SET::getGamma(expr_vector& gamma) {
    int num = m_rec_app.num_args();
    for (int i=1; i < num/2; i++) {
        gamma.push_back(m_rec_app.arg(i));
    }
}

void Predicate_SLID_SET::getX(expr_vector& x) {
    Z3_context ctx = Z3_context(z3_ctx);
    Z3_ast rec = Z3_ast(m_rec_rule);
    int n = Z3_get_quantifier_num_bound(ctx, rec);
    for (int i=n-1; i>=0; i--) {
        Z3_symbol name = Z3_get_quantifier_bound_name(ctx, rec, i);
        Z3_sort sort = Z3_get_quantifier_bound_sort(ctx, rec, i);
        Z3_ast t = Z3_mk_const(ctx, name, sort);
        x.push_back(toExpr(z3_ctx, t));
    }
}

expr Predicate_SLID_SET::getDeltaP() {
    expr_vector src_vars(z3_ctx);
    expr_vector dst_vars(z3_ctx);
    int num = m_rec_app.num_args();
    for (int i=num/2-1; i>=1; i++) {
        if (m_rec_app.arg(i).get_sort().name().str() == "SetInt") {
            src_vars.push_back(m_rec_app.arg(i));
            dst_vars.push_back(m_pars[i+num/2]);
        } else {
            break;
        }
    }
    return m_data.substitute(src_vars, dst_vars);
}

expr Predicate_SLID_SET::getUnfold1() {
    if (m_tr.hash() == z3_ctx.bool_val(false).hash()) return z3_ctx.bool_val(false);
    if (getEinGamma() != -1) {
        return !(z3_ctx.int_const((m_pars[0].to_string().c_str())) == 
            z3_ctx.int_const((m_pars[m_pars.size()/2+1].to_string().c_str()))) && m_deltap; 
    }
    return m_deltap;
}

expr Predicate_SLID_SET::getUnfold2(expr_vector& new_vars) {
    if (m_tr.hash() == z3_ctx.bool_val(false).hash()) return z3_ctx.bool_val(false);
    expr_vector gamma(z3_ctx);
    getGamma(gamma);
    int idx = getEinGamma();
    expr_vector items(z3_ctx);
    if (idx != -1) {
       string name1 = newName(gamma[idx].to_string(), new_vars.size());
       new_vars.push_back(z3_ctx.int_const(name1.c_str()));
       string name2 = newName(gamma[idx].to_string(), new_vars.size());
       new_vars.push_back(z3_ctx.int_const(name2.c_str()));
       
       expr E = z3_ctx.int_const((m_pars[0].to_string().c_str()));
       items.push_back(!(E == new_vars[0]));
       items.push_back(!(E == new_vars[1]));

    } 
    expr_vector dt_alpha(z3_ctx);
    expr_vector dt_beta(z3_ctx);
    expr_vector gamma_1(z3_ctx);
    expr_vector gamma_2(z3_ctx);
    int num = m_pars.size();
    for (int i=1; i<num/2; i++) {
        if (m_pars[i].get_sort().to_string() == "SetInt") {
            dt_alpha.push_back(m_pars[i]);
            dt_beta.push_back(m_pars[i+num/2]);

            string name1 = newName(gamma[i-1].to_string(), new_vars.size());
            new_vars.push_back(z3_ctx.constant(name1.c_str(), m_pars[i].get_sort()));
            string name2 = newName(gamma[i-1].to_string(), new_vars.size());
            new_vars.push_back(z3_ctx.constant(name2.c_str(), m_pars[i].get_sort()));
            
            expr v1 = z3_ctx.constant(name1.c_str(), gamma[i-1].get_sort());
            expr v2 = z3_ctx.constant(name2.c_str(), gamma[i-1].get_sort());
            gamma_1.push_back(v1);
            gamma_2.push_back(v2);
        }
    }
    items.push_back(m_deltap.substitute(dt_beta, gamma_1));
    items.push_back(m_deltap.substitute(dt_alpha, gamma_2));
    items.push_back(m_deltap.substitute(dt_alpha, gamma_1).substitute(dt_beta, gamma_2));

    // cout << "replace: " << m_deltap.substitute(dt_alpha, gamma_1).substitute(dt_beta, gamma_2) <<endl;

    // items.push_back(m_tr.substitute(dt_alpha, gamma_2));

    return mk_and(items);
}

int Predicate_SLID_SET::getEinGamma() {
    expr_vector gamma(z3_ctx);
    getGamma(gamma);
    for (unsigned int i=0; i<gamma.size(); i++) {
        if (m_pars[0].hash() == gamma[i].hash()) {
            return i;
        }
    }
    return -1;
}

bool Predicate_SLID_SET::getStrt() {
    expr deltap = getDeltaP();
    expr phi_r1 = deltap.arg(0);
    m_svars.push_back(phi_r1.arg(0));

    // m_case_i [00 00 0 b] [strict (max min), min or max, empty]
    m_case_i = 0; // (setunion S2 (min S1))

    int matrix[4][4]; // 0: min(S1) 1:min(S2) 2:max(S1) 3:max(S2)
    for (int i=0; i<4; i++) {
        for (int j=0; j<4; j++) {
            matrix[i][j] = INT_MAX;
        }
        matrix[i][i] = 0;
    }

    if (phi_r1.arg(1).is_const()) {
        // (= S1 S2)
        m_svars.push_back(phi_r1.arg(1));
        // min(S1) = min(S2) max(S1)=max(S2)
        matrix[0][1] = 0;
        matrix[1][0] = 0;
        matrix[2][3] = 0;
        matrix[3][2] = 0;
        m_case_i = -1;
    } else {
        expr setu = phi_r1.arg(1);
        m_svars.push_back(setu.arg(0));
        if (setu.arg(1).decl().name().str() == "setunion") {
            // (setunion S1 (setunion (min S1) (max S1)))
            m_case_i = 5; // [xx 10 1]
        } else if (setu.arg(1).arg(0).decl().name().str() == "max") {
            // (setunion S1 (max S1))
            m_case_i = 2; // [xx 01 x]
        }
    }

    bool has_s1 = false;
    bool has_s2 = false;

    for (unsigned int i=1; i<m_deltap.num_args(); i++) {
        expr phi_r2_i = m_deltap.arg(i);
        if (phi_r2_i.is_app()) {
            int c = 0;
            expr item1 = phi_r2_i.arg(0);
            expr item2 = phi_r2_i.arg(1);
            if (item2.is_app()) {
                if (item2.decl().name().str() == "+") {
                    c = item2.arg(1).get_numeral_int();
                    item2 = item2.arg(0);
                } else if (item2.decl().name().str() == "-") {
                    c = -item2.arg(1).get_numeral_int();
                    item2 = item2.arg(0);
                }
            }
            int row = getCard(item1, m_svars);
            int col = getCard(item2, m_svars);

            if (((row&1)&(col&1)) == 0) has_s1 = true;
            if (((row&1)|(col&1)) == 1) has_s2 = true;

            if (phi_r2_i.decl().name().str() == "=") {
                setMatrix(matrix, row, col, c);
                setMatrix(matrix, col, row, -c);
            } else if (phi_r2_i.decl().name().str() == "<=") {
                setMatrix(matrix, row, col, c);
            } else if (phi_r2_i.decl().name().str() == ">=") {
                setMatrix(matrix, col, row, -c);
            }
        }
    }
    if (has_s1) {
        // min(S1) <= max(S1)
        setMatrix(matrix, 0, 2, 0);
    }
    if (has_s2) {
        // min(S2) <= max(S2)
        setMatrix(matrix, 1, 3, 0);
        if (m_case_i != -1) {
            // s2 noempty
            m_case_i |= 1; // [xx xx 1]

            setMatrix(matrix, 0, 1, 0); // min(S1) <= min(S2)
            setMatrix(matrix, 3, 2, 0); // max(S2) <= max(S1)

            if ((m_case_i & 6) == 0) {
                // [xx 00 x]
                setMatrix(matrix, 2, 3, 0); // max(S1) <= max(S2)
            } else if ((m_case_i & 6) == 2) {
                // [xx 01 x]
                setMatrix(matrix, 1, 0, 0); // min(S2) <= min(S1)
            }
        } 
    }

    bool is_sat = floyd(matrix);

    if (is_sat) {
        if (matrix[0][1] < 0) {
            m_case_i |= 16; // min(S1) <= min(S2) - c (c > 0)
        }
        if (matrix[3][2] < 0) {
            m_case_i |= 8; // max(S2) <= max(S1) - c (c > 0)
        }

        // strict result
        for (int i=0; i<3; i++) {
            for (int j=i+1; j<4; j++) {

                m_strt_items.push_back(getIJExpr(matrix, i, j, m_svars));
                // cout << i << "," << j << " --> " << matrix[i][j] << " -- " << m_strt_items.back() << endl;
            }
        }
    }
    return is_sat;
}


expr Predicate_SLID_SET::getTr() {
    bool is_sat = getStrt();

    if (is_sat) {
        initStrtPars();
        expr_vector and_items(z3_ctx);
        and_items.push_back(!(m_svars[0] == m_svars[1]));
        if ((m_case_i & 1) == 0) {
            and_items.push_back(!getTrPossiblelyEmpty());
        } else {
            and_items.push_back(!getTrSurelyNonempty());
        }
        return !mk_and(and_items);
    } else {
        return z3_ctx.bool_val(false);
    }
}


expr Predicate_SLID_SET::apply(expr_vector& args) {
    func_decl fp = m_rec_app.decl();
    return fp(args);
}

void Predicate_SLID_SET::initStrtPars() {
    expr S1 = m_svars[0];
    expr S2 = m_svars[1];
    if ((m_case_i & 6) == 0) {
        // min
        m_strt_pars.push_back(z3_buffer.getMin(S1));
        m_strt_pars.push_back(z3_buffer.getMin(S2));
    } else if((m_case_i&6) == 2) {
        // max
        m_strt_pars.push_back(z3_buffer.getMax(S1));
        m_strt_pars.push_back(z3_buffer.getMax(S2));
    }


}

expr Predicate_SLID_SET::subPhiR2(expr e1, expr e2) {
    expr_vector dst(z3_ctx);
    if (e1.is_const()) {
        dst.push_back(getDataItem(e1)); 
    } else {
        dst.push_back(e1);
    }

    if (e2.is_const()) {
        dst.push_back(getDataItem(e2));
    } else {
        dst.push_back(e2);
    }

    if ((m_case_i&6) == 0) {
        return m_strt_items[0].substitute(m_strt_pars, dst);
    } else if ((m_case_i&6) == 2) {
        return m_strt_items[5].substitute(m_strt_pars, dst);
    }
    return z3_ctx.bool_val(true);
}

expr Predicate_SLID_SET::getDataItem(expr e) {
    if ((m_case_i & 6) == 0) {
        return z3_buffer.getMin(e);
    } else {
        return z3_buffer.getMax(e);
    }
}

void Predicate_SLID_SET::initSucc() {
    expr_vector and_items(z3_ctx);
    expr S = z3_ctx.constant("S", z3_buffer.getSort("SetInt"));
    expr x = z3_ctx.int_const("x");
    expr y = z3_ctx.int_const("y");
    m_succ_pars.push_back(S);
    m_succ_pars.push_back(x);
    m_succ_pars.push_back(y);

    and_items.push_back(z3_buffer.getBelongsto(x, S));
    and_items.push_back(z3_buffer.getBelongsto(y, S));
    expr_vector pars(z3_ctx);
    expr z = z3_ctx.int_const("z");
    pars.push_back(z);
    expr ebody = z3_buffer.getBelongsto(z, S) 
        && !(z >= x + 1 && y >= z + 1);
    and_items.push_back(forall(pars, ebody));
    m_succ = mk_and(and_items);
}

void Predicate_SLID_SET::show() {

    cout << "pars: \n";
    for (unsigned int i=0; i<m_pars.size(); i++) {
        cout << m_pars[i] << " ";
    }
    cout << endl;

    cout << "base: " << m_base_rule << endl;
    cout << "rec: " << m_rec_rule << endl;

    // z3_buffer.show();

    cout << "Details: \n";
    expr_vector x(z3_ctx);
    getX(x);
    expr_vector rx(z3_ctx);
    for (unsigned int i=0; i<x.size(); i++) {
        cout << x[i] << " ";
    }
    cout << endl;

    cout << "pto: " << m_pto <<endl;
    cout << "recursive app: " << m_rec_app <<endl;
    cout << "data formula: " << m_data << endl;

    expr_vector alpha(z3_ctx);
    getAlpha(alpha);
    expr_vector beta(z3_ctx);
    getBeta(beta);
    expr_vector gamma(z3_ctx);
    getGamma(gamma);
    
    cout <<"alpha: " << alpha << endl;
    cout << "beta: " << beta << endl;
    cout << "gamma: " << gamma << endl;

    cout << "deltaP: " << m_deltap <<endl;

    expr_vector m_svars(z3_ctx);
    expr_vector m_strt_items(z3_ctx);
    int m_case_i = 0;
    bool is_sat = getStrt();
    cout << "is_sat: " << is_sat << endl;
    cout << "m_case_i: " << m_case_i << endl;
    cout << "strt: " << m_strt_items << endl;
    
    cout << "tr closure: \n";
    expr tr = getTr();
    // cout << tr <<endl;
}

expr Predicate_SLID_SET::getTrPossiblelyEmpty() {
    expr_vector evars(z3_ctx);
    expr ES = z3_ctx.constant("ES", z3_buffer.getSort("SetInt"));
    expr ES1 = z3_ctx.constant("ES1", z3_buffer.getSort("SetInt"));
    evars.push_back(ES);
    evars.push_back(ES1);

    expr S = m_svars[0];

    expr_vector and_items(z3_ctx);

    expr emptyset = z3_buffer.getEmptyset();

    and_items.push_back(!(ES == emptyset));
    and_items.push_back(m_strt_items[1]); // min(S) max(S)
    expr_vector src_pars(z3_ctx);
    src_pars.push_back(S);
    expr_vector dst_pars(z3_ctx);
    dst_pars.push_back(ES);
    expr phip = m_deltap.arg(0) && m_strt_items[1];
    and_items.push_back(phip.substitute(src_pars, dst_pars));

    and_items.push_back(z3_buffer.getSubset(ES, S));


    if ((m_case_i & 6) == 4) {
        expr ES2 = z3_ctx.constant("ES2", z3_buffer.getSort("SetInt"));
        evars.push_back(ES2);
        expr setu = z3_buffer.getSetunion(ES1, ES);
        and_items.push_back(S == z3_buffer.getSetunion(setu, ES2)); // S = ES1 union ES union ES2
        and_items.push_back(!(!(ES1==emptyset) && z3_buffer.getMax(ES1) >= z3_buffer.getMin(ES)));
        and_items.push_back(!(!(ES2==emptyset) && z3_buffer.getMin(ES2) <= z3_buffer.getMax(ES)));
    } else {
        expr setminus = z3_buffer.getSetminus(S, ES);
        and_items.push_back(ES1 == setminus);
        if ((m_case_i & 6) == 0) {
            // min
            and_items.push_back(!(!(ES1==emptyset) && z3_buffer.getMax(ES1) >= z3_buffer.getMin(ES)));
        } else if ((m_case_i & 6) == 2) {
            // max
            and_items.push_back(!(!(ES1==emptyset) && z3_buffer.getMin(ES1) <= z3_buffer.getMax(ES)));
        }
    }
    return !forall(evars, !mk_and(and_items));
}

expr Predicate_SLID_SET::getTrSurelyNonempty() {
    expr_vector and_items(z3_ctx);
    and_items.push_back(!m_deltap);
    and_items.push_back(!getUnfoldDeltap2());
    and_items.push_back(!getUnfoldDeltap3());
    expr or_item4 = getUnfoldDeltap3();
    return !(mk_and(and_items)); 
}

expr Predicate_SLID_SET::getUnfoldDeltap2() {
    expr_vector evars(z3_ctx);
    expr nvar = z3_ctx.constant("ES", z3_buffer.getSort("SetInt"));
    evars.push_back(nvar);
    expr_vector vars1(z3_ctx);
    vars1.push_back(m_svars[0]);
    expr_vector vars2(z3_ctx);
    vars2.push_back(m_svars[1]);
    return exists(evars, m_deltap.substitute(vars1, evars) 
        && m_deltap.substitute(vars2, evars));
}

expr Predicate_SLID_SET::getUnfoldDeltap3() {
    expr_vector evars(z3_ctx);
    expr ES1 = z3_ctx.constant("ES1", z3_buffer.getSort("SetInt"));
    expr ES2 = z3_ctx.constant("ES2", z3_buffer.getSort("SetInt"));
    expr ES3 = z3_ctx.constant("ES3", z3_buffer.getSort("SetInt"));
    evars.push_back(ES1);
    evars.push_back(ES2);
    evars.push_back(ES3);

    expr S1 = m_svars[0];
    expr S2 = m_svars[1];

    expr_vector svars1(z3_ctx);
    svars1.push_back(S1);
    expr_vector dvars1(z3_ctx);
    dvars1.push_back(ES1); 

    expr_vector svars2(z3_ctx);
    svars2.push_back(S2);
    expr_vector  dvars2(z3_ctx);
    dvars2.push_back(ES2);

    expr phi_r = m_deltap.arg(0) && m_strt_items[0] && m_strt_items[1] 
        && m_strt_items[4] && m_strt_items[5];
    
    expr_vector and_items(z3_ctx);
    and_items.push_back(phi_r.substitute(svars2, dvars1)); // (S1, ES1)
    and_items.push_back(phi_r.substitute(svars1, dvars2)); // (ES2, S2)

    expr emptyset = z3_buffer.getEmptyset();
    and_items.push_back(!(ES2 == emptyset)); // ES2 != empty

    // strict item
    // min min
    expr A = !(ES3 == emptyset); 
    expr B = z3_buffer.getMax(ES3) <= z3_buffer.getMin(ES2) - 1;
    expr min_strict_item = A && B;
    // max max 
    B = z3_buffer.getMax(ES2) <= z3_buffer.getMin(ES3) - 1;
    expr max_strict_item = A && B;

    // non-strict item
    // min min
    B = z3_buffer.getMax(ES3) >= z3_buffer.getMin(ES2);
    expr min_non_strict_item = !(A && B);
    // max max
    B = z3_buffer.getMax(ES2) >= z3_buffer.getMin(ES3);
    expr max_non_strict_item = !(A && B);

    // succ item
    expr_vector epars(z3_ctx);
    expr x = z3_ctx.int_const("x");
    expr y = z3_ctx.int_const("y");
    epars.push_back(x);
    epars.push_back(y);

    expr_vector succ_vars(z3_ctx);
    expr_vector rpars(z3_ctx);
    // min
    expr min_s2 = z3_buffer.getMin(ES2);
    expr set_item = z3_buffer.getSet(min_s2);
    expr union_item = z3_buffer.getSetunion(ES3, set_item);
    succ_vars.push_back(union_item);
    succ_vars.push_back(x);
    succ_vars.push_back(y);

    rpars.push_back(z3_buffer.getMin(S1));
    rpars.push_back(z3_buffer.getMin(S2));

    expr min_succ = forall(epars, !(m_succ.substitute(m_succ_pars, succ_vars) 
        && !(m_strt_items[0].substitute(rpars, epars))));

    // max
    succ_vars.resize(0);
    rpars.resize(0);

    expr max_s2 = z3_buffer.getMax(ES2);
    set_item = z3_buffer.getSet(max_s2);
    union_item = z3_buffer.getSetunion(ES3, set_item);
    succ_vars.push_back(union_item);
    succ_vars.push_back(x);
    succ_vars.push_back(y);

    rpars.push_back(z3_buffer.getMax(S2));
    rpars.push_back(z3_buffer.getMax(S1));

    expr max_succ = forall(epars, !(m_succ.substitute(m_succ_pars, succ_vars) 
        && !(m_strt_items[5].substitute(rpars, epars))));

    if ((m_case_i & 6) == 4) {
        // cout << "min-max case: \n";
        // min max
        expr ES4 = z3_ctx.constant("ES4", z3_buffer.getSort("SetInt"));
        evars.push_back(ES4);

        expr setu = z3_buffer.getSetunion(ES2, ES3);
        and_items.push_back(ES1 == z3_buffer.getSetunion(setu, ES4)); // ES1 = ES3 union ES2 union ES4

        expr_vector s3par(z3_ctx);
        s3par.push_back(ES3);

        expr_vector s4par(z3_ctx);
        s4par.push_back(ES4);

        if ((m_case_i&16) != 0) {
            // min strict
            and_items.push_back(min_strict_item);
        } else {
            and_items.push_back(min_non_strict_item);
        }

        if ((m_case_i&8) != 0) {
            // max strict
            and_items.push_back(max_strict_item.substitute(s3par, s4par));
        } else {
            and_items.push_back(max_non_strict_item.substitute(s3par, s4par));
        }

        // succ item
        and_items.push_back(min_succ);
        and_items.push_back(min_succ.substitute(s3par, s4par));

        // quantelmt
        m_free_item = z3_buffer.getQuantElmt(m_strt_items[0], m_strt_items[5]);
        // cout << "free: " << m_free_item <<endl;
        and_items.push_back(m_free_item);
    } else {
        // min or max
        expr setminus = z3_buffer.getSetminus(ES1, ES2);
        and_items.push_back(ES3 == setminus);
        and_items.push_back(z3_buffer.getSubset(ES2, ES1));

        if ((m_case_i&6) == 0) {
            // min
            if ((m_case_i&16) != 0) {
                // strict
                and_items.push_back(min_strict_item);
            } else {
                // non-strict
                and_items.push_back(min_non_strict_item);
            }

            // succ item
            
        } else if ((m_case_i&6) == 2) {
            // max
            if ((m_case_i&8) != 0) {
                // strict
                and_items.push_back(max_strict_item);
            } else {
                // non-strict
                and_items.push_back(max_non_strict_item);
            }
       }
    }

    return !forall(evars, !mk_and(and_items));
}

int Predicate_SLID_SET::getCard(expr& item, expr_vector& m_svars) {
    int index = 0;
    if (item.decl().name().str() == "max") {
        index += 2;
    }

    if (item.arg(0).hash() == m_svars[1].hash()) {
        index += 1;
    }
    return index;
}

void Predicate_SLID_SET::setMatrix(int (&matrix)[4][4], int i, int j, int val) {
    if (matrix[i][j] > val) {
        matrix[i][j] = val;
    }
}

bool Predicate_SLID_SET::floyd(int (&matrix)[4][4]) {

    int path[4][4];
    int dist[4][4];

    // init path and dist
    for (int i=0; i<4; i++) {
            for (int j=0; j<4; j++){
                    path[i][j] = j;
                    dist[i][j] = matrix[i][j];
            }
    }

    // compute the shortest path
    int tmp;
    for (int i=0; i<4; i++) {
            for (int row=0; row<4; row++) {
                    for (int col=0; col<4; col++) {
                            tmp = (dist[row][i] == INT_MAX || dist[i][col]==INT_MAX)? INT_MAX : dist[row][i] + dist[i][col];
                            if (dist[row][col] > tmp) {
                                    dist[row][col] = tmp;
                                    path[row][col] = path[row][i];
                            }
                    }
            }
    }

    // check whether negative cycle exists or not
    for (int i=0; i<4; i++) {
            for (int row=0; row<4; row++) {
                    for (int col=0; col<4; col++) {
                            tmp = (dist[row][i] == INT_MAX || dist[i][col]==INT_MAX)? INT_MAX : dist[row][i] + dist[i][col];
                            if (dist[row][col] > tmp) {
                                    return false;
                            }
                    }
            }
    }

    for (int i=0; i<4; i++) {
            for (int j=0; j<4; j++){
                    matrix[i][j] = dist[i][j];
            }
    }
    return true;
}

expr Predicate_SLID_SET::getIJExpr(int (&matrix)[4][4], int i, int j, expr_vector& m_svars) {
    // expr phi_ij = z3_ctx.bool_val(true);
    expr_vector and_items(z3_ctx);
    if (i != j) {
        if (matrix[i][j] != INT_MAX && -matrix[i][j] == matrix[j][i]) {
            if (matrix[i][j] < 0) {
                and_items.push_back(getIExpr(j, m_svars) == getIExpr(i, m_svars) + z3_ctx.int_val(-matrix[i][j]));
            } else {
                and_items.push_back(getIExpr(i, m_svars) == getIExpr(j, m_svars) + z3_ctx.int_val(matrix[i][j]));
            }
        } else {
            if (matrix[i][j] != INT_MAX) {
                if (matrix[i][j] < 0) {
                    and_items.push_back(getIExpr(j, m_svars) >= getIExpr(i, m_svars) + z3_ctx.int_val(-matrix[i][j]));
                } else {
                    and_items.push_back(getIExpr(i, m_svars) <= getIExpr(j, m_svars) + z3_ctx.int_val(matrix[i][j]));
                }
            }
            if (matrix[j][i] != INT_MAX) {
                if (matrix[j][i] < 0) {
                    and_items.push_back(getIExpr(i, m_svars) >= getIExpr(j, m_svars) + z3_ctx.int_val(-matrix[j][i]));
                } else {
                    and_items.push_back(getIExpr(j, m_svars) <= getIExpr(i, m_svars) + z3_ctx.int_val(matrix[j][i]));
                }
            }
            
        }
    }
    if (and_items.size() > 0) {
        return mk_and(and_items);
    } else {
        return z3_ctx.bool_val(true);
    }
}

expr Predicate_SLID_SET::getIExpr(int i, expr_vector& m_svars) {
    string key = "_SetInt_Int";
    if ((i&2) == 0) {
        key = "min" + key;
    } else {
        key = "max" + key;
    }
    func_decl f = z3_buffer.getFuncDecl(key);
    return f(m_svars[i&1]);
}


string Predicate_SLID_SET::newName(string name, int i) {
    int idx;
    if ((idx = name.find("?")) == -1) {
        return name + "_N" + to_string(i);
    }
    return name.replace(idx, 1, "") + "_N" + to_string(i);
}
