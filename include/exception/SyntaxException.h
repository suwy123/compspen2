/*******************************************
*  @file  SyntaxException.h                * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#if !defined(SYNTAX_EXCEPTION_)
#define SYNTAX_EXCEPTION_

#include "BaseException.h"

/*! @class SyntaxException
 *  @brief Brief class description
 *
 *  Detailed description
 */
class SyntaxException : public BaseException
{
public:
    SyntaxException(string info, int row=-1, int col=-1)
        : BaseException("SYNTAX_ERROR", info, row, col) {}
    virtual ~SyntaxException() {}
 
};

#endif
