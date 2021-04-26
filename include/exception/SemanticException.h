/*******************************************
*  @file  SemanticException.h              * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/
#if !defined(SEMANTIC_EXCEPTION_)
#define SEMANTIC_EXCEPTION_

#include "BaseException.h"

/*! @class SemanticException
 *  @brief Brief class description
 *
 *  Detailed description
 */
class SemanticException : public BaseException
{
public:
    SemanticException(string info, int row=-1, int col=-1)
        :BaseException("SEMANTIC ERROR", info, row, col) {}
    virtual ~SemanticException() {}

};

#endif
