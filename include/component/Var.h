#if !defined(VAR_)
#define VAR_


#include "Types.h"
#include "component/SortType.h"
#include "z3++.h"

using namespace z3;

/*! @class Var
 *  @brief Brief class description
 *
 *  Detailed description
 */
class Var
{
public:
    Var() {}
    Var(string var_name, SortType* sort): m_var_name(var_name), m_sort_type(sort) {}
    virtual ~Var() {}

    string getName() {return m_var_name;}
    SortType* getSort() {return m_sort_type;}
    operator expr();

    void show() {cout << m_var_name << " "; if (m_sort_type != nullptr) cout << m_sort_type->getName() << " ";}

protected:
    string m_var_name; ///< Member description
    SortType* m_sort_type; ///< Sort
};

#endif
