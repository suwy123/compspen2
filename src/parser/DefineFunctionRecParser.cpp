/*******************************************
*  @file  DefineFunctionRecParser.cpp      *
*  @brief  Brief file description          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "parser/DefineFunctionRecParser.h"
#include "component/Z3Buffer.h"
#include "component/Predicate.h"
#include "component/Predicate_SLID_SET.h"
#include "component/Predicate_SLID_INT.h"
#include "component/Predicate_SLAH.h"

extern SyntaxErrorTable SYNTAX_ERROR_INFO;
//extern Z3Buffer z3_buffer; 
//extern z3::context z3_ctx;

void DefineFunctionRecParser::parse(Parser& parser) {
    Token* curr = parser.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
    string fname = dynamic_cast<StrToken*>(curr)->value();
    parseParameters(parser);

    FuncType* pf = new FuncType(z3_ctx, z3_buffer, fname);

    VarList vpars;
    parser.topVar(vpars);
    expr_vector pars(z3_ctx);
    z3::sort_vector domain(z3_ctx);//
    for (auto par : vpars) {
        pars.push_back(z3_buffer.getVar(par));
        pf->addArg(par->getSort()->getName());
        domain.push_back(z3_buffer.getSort(par->getSort()));//
    }

    SortType* range = parseSort(parser);

    pf->addArg(range->getName());

    parser.addFunc(fname, pf);
    
    z3::symbol fname_sym = z3_ctx.str_symbol(fname.c_str());//
    z3::sort rangesort = z3_buffer.getSort(range);
    z3::func_decl fun = z3_ctx.function(fname_sym, domain, rangesort);//

    // parse body
    parser.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);
    curr = parser.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
    string or_op = dynamic_cast<StrToken*>(curr)->value();
    if (or_op != "or") {
        throw SemanticException("the body must be 'or' function!", curr->row(), curr->col());
    }
    
    // parser.showEnv();
    // base rule
    parseExpr(parser);
    expr base = parser.topArg();
    parser.popArg();

    parser.checkNext(LEFT_PAREN, SYNTAX_ERROR_INFO[LEFT_PAREN]);
    curr = parser.checkNext(SYMBOL_TOKEN, SYNTAX_ERROR_INFO[SYMBOL_TOKEN]);
    string exists_op = dynamic_cast<StrToken*>(curr)->value();
    if (exists_op != "exists") {
        throw SemanticException("the rule must be exists function!", curr->row(), curr->col());
    }
    // recursive rule
    parseExists(parser);

    z3::expr rec = parser.topArg();
    parser.popArg();

    parser.checkNext(RIGHT_PAREN, SYNTAX_ERROR_INFO[RIGHT_PAREN]);
    // action
    parser.popVar();

    parser.checkNext(RIGHT_PAREN, SYNTAX_ERROR_INFO[RIGHT_PAREN]);
    // action 
    parser.popVar(); 

    // TODO generate predicate definition
    Predicate* pred;
    if(parser.getProblem()->getLogic() == "QF_SLID_SET"){ 
    	pred = new Predicate_SLID_SET(z3_ctx, z3_buffer, pars, base, rec);
	}else if(parser.getProblem()->getLogic() == "QF_SLID_INT"){
		pred = new Predicate_SLID_INT(z3_ctx, fun, pars, base, rec);
	}else if(parser.getProblem()->getLogic() == "QF_SLAH"){
		checkSLAHRecRule(parser, rec, fname);
		pred = new Predicate_SLAH(z3_ctx, fun, pars, base, rec);
	}else{
		pred = new Predicate(z3_ctx, pars, base, rec);
	}
    parser.addPredicate(pred);
//cout<<"define-fun-rec done"<<endl;
}

void DefineFunctionRecParser::checkSLAHRecRule(Parser& parser, z3::expr rec, string fname){
	z3::expr body = rec.body();
	if (!body.is_app() || body.decl().name().str() != "sep") {
        throw SemanticException("'sep' is excepted in the recursive rule");
	}
	if (body.num_args() != 2) {
        throw SemanticException("two args is excepted in sep of the recursive rule");
	}
	if(parser.getProblem()->getHeapChunk() == nullptr){
		throw SemanticException("'hck' is not defined");
	}
	if(body.arg(0).decl().name().str() != parser.getProblem()->getHeapChunk()->get_name()){
		throw SemanticException("'hck' is excepted in the recursive rule");
	}
	if(body.arg(1).decl().name().str() != fname){
		string err_info = fname+" is excepted in the recursive rule";
		throw SemanticException(err_info);
	}
}
