#if !defined(FUNC_TYPE_)
#define FUNC_TYPE_
/*******************************************
*  @file  FuncType.h                       * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "component/SortType.h"
#include "Types.h"
#include "z3++.h"


using ArgTypeList = vector<SortType*>;
using ParTypeList = vector<string>;
using ParTypeSet = set<string>;
using FuncDeclBucket = map<string, string>;

/*! @class FuncType
 *  @brief Brief class description
 *
 *  Detailed description
 */
class FuncType
{
public:
    FuncType(string name, string attr="", bool determine=true)
        :m_name(name), m_attr(attr), m_determine(determine){}
    virtual void addArg(string arg) {m_arg_list.push_back(arg);}
    virtual void addPar(string par) {}
    void setAttr(string attr) {m_attr = attr;}
    virtual ~FuncType() {}

    virtual string getName() {return m_name;}
    virtual string getRange() {return m_arg_list.back();}
    virtual bool isDetermine() {return m_determine;}

    virtual z3::func_decl determine(ArgTypeList& arg_type_list);

    ParTypeList& getArgList() { return m_arg_list; }

    virtual void show(); 

protected:
    void genStr(ArgTypeList& alist, string& result) {
        ostringstream oss;
        for (auto item : alist) {
            oss << item->getName() << "_";
        }

        result = oss.str();
    }

protected:
    string m_name; ///< Member description
    ParTypeList m_arg_list; ///< parameter list
    string m_attr; ///< associate type
    bool m_determine; ///< arg_list whether determine
    FuncDeclBucket m_func_decl_bucket; ///< actual function declaration
    
};



/*! @class ParFuncType
 *  @brief Brief class description
 *
 *  Detailed description
 */
class ParFuncType :public FuncType
{
public:
    ParFuncType(string name):FuncType(name, "", false) {}
    virtual ~ParFuncType() {}
    virtual void addPar(string par) {m_par_set.insert(par);}
    virtual void addArg(string arg); 
    virtual z3::func_decl determine(ArgTypeList& arg_type_list);
    virtual void show(); 

protected:
    ParTypeSet m_par_set; ///< Member description
    vector<bool> m_par_arg_mark_list; ///< mark the same position argument
};

#endif
