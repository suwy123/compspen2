#include "z3++.h"
#include "slah_api.h"
#include <iostream>

int main(){
    z3::context z3_ctx;
    z3::expr x1 = z3_ctx.int_const("x1");
    z3::expr x2 = z3_ctx.int_const("x2");
    z3::expr x3 = z3_ctx.int_const("x3");
    z3::expr x4 = z3_ctx.int_const("x4");
    z3::expr pto = slah_api::newPto(x1,x2);
    z3::expr blk = slah_api::newBlk(x3,x4);
    z3::expr emp = slah_api::newEmp(z3_ctx);

    z3::expr spatialFormula = slah_api::sep(pto,blk);
    spatialFormula = slah_api::sep(spatialFormula,emp);
    z3::expr phi1 = (x1 + 1) == x3 && spatialFormula;
    z3::expr phi2 = x3 == x4 && blk;
    z3::expr phi = (x3 == 1 && phi2) || (x3 > 1 && phi1);

    std::cout<<phi<<" checkSat"<<std::endl;
    if(slah_api::checkSat(phi) == z3::sat){
        std::cout<<"result: sat"<<std::endl;
    }else{
        std::cout<<"result: unsat"<<std::endl;
    }

    z3::expr psi = slah_api::newBlk(x1,x4);
    std::cout<<phi1<<" |= "<<psi<<std::endl;
    if(slah_api::checkEnt(phi1,psi) == z3::sat){
        std::cout<<"result: sat"<<std::endl;
    }else{
        std::cout<<"result: unsat"<<std::endl;
    }
    return 0;
}
