#if !defined(THEORY_PARSER_)
#define THEORY_PARSER_
/*******************************************
*  @file  TheoryParser.h                   * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "CommandParser.h"
#include "component/Z3Buffer.h"
#include "Types.h"

/*! @class TheoryParser
 *  @brief Brief class description
 *
 *  Detailed description
 */
class TheoryParser
{
public:
	z3::context& z3_ctx;
	Z3Buffer& z3_buffer;
    TheoryParser(z3::context& ctx, Z3Buffer& buffer, string theory): z3_ctx(ctx), z3_buffer(buffer), m_theory(theory) {}
    virtual ~TheoryParser() {}
    virtual void parse(Parser& parser);

protected:
    string m_theory; //<<< theory name
    const string m_prefix="/home/swy/workspace/compspen/logic_def/Theories/";
    TokenScannerFactory m_factory;
    TheoryInfo m_info;
};

#endif
