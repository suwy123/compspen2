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
#include "Types.h"

/*! @class TheoryParser
 *  @brief Brief class description
 *
 *  Detailed description
 */
class TheoryParser
{
public:
    TheoryParser(string theory): m_theory(theory) {}
    virtual ~TheoryParser() {}
    virtual void parse(Parser& parser);

protected:
    string m_theory; //<<< theory name
    const string m_prefix="/home/swy/compspen2/theorylogic/Theories/";
    TokenScannerFactory m_factory;
    TheoryInfo m_info;
};

#endif
