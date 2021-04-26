#if !defined(BITVECTOR_LITERAL_SCANNER_)
#define BITVECTOR_LITERAL_SCANNER_
/*******************************************
*  @file  BitVectorLiteralScanner.h        * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include "TokenScanner.h"

/*! @class BitVectorLiteralScanner
 *  @brief Brief class description
 *
 *  Detailed description
 */
class BitVectorLiteralScanner: public TokenScanner
{
public:
    BitVectorLiteralScanner();
    virtual ~BitVectorLiteralScanner();
    Token* scan(Scanner& scanner);
};
#endif
