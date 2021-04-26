#if !defined(COMMENT_SCANNER_)
#define COMMENT_SCANNER_
/*******************************************
*  @file  CommentScanner.h                 * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "TokenScanner.h"

/*! @class CommentScanner
 *  @brief Brief class description
 *
 *  Detailed description
 */
class CommentScanner :public TokenScanner
{
public:
    CommentScanner() {}
    virtual ~CommentScanner() {}
    Token* scan(Scanner& scanner); 
};

#endif
