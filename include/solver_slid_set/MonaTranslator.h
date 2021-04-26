#ifndef MONA_TRANSLATOR_H
#define MONA_TRANSLATOR_H

#include <map>
#include "component/Z3Buffer.h"

enum mona_op {
    MONA_AND=1,
    MONA_OR,
    MONA_NOT,
    MONA_IMPLY,
    MONA_LE,
    MONA_LT,
    MONA_GE,
    MONA_GT,
    MONA_EQ,
    MONA_DISTINCT,
    MONA_SUB,
    MONA_IN,
    MONA_SET,
    MONA_PLUS,
    MONA_SUBSTRACT,
    // MONA_MUL,
    // MONA_DIV,
    MONA_MIN,
    MONA_MAX,
    MONA_UNION,
    MONA_INTER,
    MONA_MINUS
};



class MonaTranslator {
private:
    z3::expr m_formula;
    std::map<std::string, mona_op> op_map;
private:
    std::string getDeclStr(int order, std::set<z3::expr, exprcomp>& var_set);
    std::string getQuantifierBoundStr(bool is_all, z3::expr_vector& bounds);
public:
    MonaTranslator(z3::expr formula);
    void writeToFile(std::string name);
    std::string getStr(z3::expr item, std::set<std::string>& set_items);
};

#endif /* MONA_TRANSLATOR_H */
