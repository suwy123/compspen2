#if !defined(PREDICATE_SLID_SET_)
#define PREDICATE_SLID_SET_
/******************************************
*  @file  Predicate_SLID_SET.h             *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2019-2-18                     *
*                                          *
*******************************************/

#include "z3++.h"
#include "Types.h"
#include "component/Predicate.h"

using namespace z3;

class Predicate_SLID_SET:public Predicate {
public:
    Predicate_SLID_SET(z3::expr_vector pars, z3::expr base, z3::expr rec);
    virtual ~Predicate_SLID_SET();

    void getABC(expr_vector& alpha, expr_vector& beta, expr_vector& gamma);
    void getAlpha(expr_vector& alpha);
    void getBeta(expr_vector& beta);
    void getGamma(expr_vector& gamma);
    void getX(expr_vector& x);
    expr_vector& getPars() {return m_pars;}
    expr getFreeItem() {return m_free_item;}

    expr getPhip() {return m_deltap;}

    expr getUnfold1();
    expr getUnfold2(expr_vector& new_vars);

    expr subPhiR2(expr e1, expr e2);
    expr getDataItem(expr e1);
    int getCase() {return m_case_i;}

    int getEinGamma();
    bool getStrt();
    expr getTr();

    expr apply(expr_vector& args);
    expr unfoldPredicate(expr_vector& args);

    virtual void show();

private:
    expr getDeltaP();

    expr getTrPossiblelyEmpty();
    expr getTrSurelyNonempty();
    expr getUnfoldDeltap2();
    expr getUnfoldDeltap3();

    void initSucc();
    void initStrtPars();
    int getCard(expr& var, expr_vector& svars);
    void setMatrix(int (&matrix)[4][4], int i, int j, int val);
    bool floyd(int (&matrix)[4][4]);
    expr getIJExpr(int (&matrix)[4][4], int i, int j, expr_vector& svars);
    expr getIExpr(int i, expr_vector& svars);

    string newName(string name, int i);

private:
    expr m_data;
    expr m_pto;
    expr m_rec_app;
    expr m_deltap;

    expr m_succ;
    expr_vector m_succ_pars;

    int m_case_i;
    expr_vector m_svars;
    expr_vector m_strt_items;
    expr_vector m_strt_pars;

    expr m_tr;
    expr m_free_item;
};

#endif
