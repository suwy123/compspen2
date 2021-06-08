#if !defined(LOGIC_PARSER_)
#define LOGIC_PARSER_
/*******************************************
*  @file  LogicParser.h                    * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "Parser.h"


/*! @class LogicParser
 *  @brief Brief class description
 *
 *  Detailed description
 */
class LogicParser
{
public:
    LogicParser(string logic): m_logic(logic) {m_theories.push_back("Core");}
    virtual ~LogicParser() {}
    Theories& getTheories() {return m_theories;}
    LogicInfo& getLogicInfo() {return m_info;}

    virtual void parse(Parser& parser); 

    protected:
    string m_logic; ///< Member description
    const string m_prefix="/home/swy/workspace/compspen/logic_def/Logics/";
    TokenScannerFactory m_factory;
    
    LogicInfo m_info;
    Theories m_theories;
};

#endif
