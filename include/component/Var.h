#if !defined(VAR_)
#define VAR_


#include "Types.h"
#include "component/SortType.h"
#include "component/Z3Buffer.h"
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
	z3::context& z3_ctx;
	Z3Buffer& z3_buffer;
    Var(z3::context& ctx, Z3Buffer& buffer): z3_ctx(ctx), z3_buffer(buffer) {}
    Var(z3::context& ctx, Z3Buffer& buffer, string var_name, SortType* sort)
	: z3_ctx(ctx), z3_buffer(buffer), m_var_name(var_name), m_sort_type(sort) {}
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
