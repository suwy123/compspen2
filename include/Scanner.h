#if !defined(SCANNER_)
#define SCANNER_
/*******************************************
*  @file     Scanner.h                     *
*  @brief    Scanner for smt file          *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-11-7                     *
*                                          *
*******************************************/

#include<iostream>
#include<vector>

#include "Token.h"
#include "exception/SyntaxException.h"

using std::istream;
using std::string;
using std::vector;

/*! @class Scanner
 *  @brief Brief class description
 *
 *  Detailed description
 */
class Scanner 
{
public:
    Token* checkNext(TOKEN type, string info);
    Token* nextToken();

public:
    Scanner(istream& in);
    ~Scanner() {}
    bool next();
    bool skip();
    void newLine() {m_line++; m_position=0;}
    char curr() const {return m_curr;}
    bool iseof() const {return m_is_eof;}
    int line() const {return m_line;}
    int col() const {return m_position;}
    void back(int offset) {if (m_curr == '\n') {m_line--;} else {m_position--;} m_in.seekg(offset, std::ios_base::cur);}

    void clearStrCache() {m_str_cache.clear();}
    void cache(char ch) {m_str_cache.push_back(ch);}
    const vector<char>& getCache() const { return m_str_cache;}

protected:

    int m_line; ///< line number
    int m_position; ///< position in line
    char m_curr; ///< current char

    bool m_is_eof; ///< EOF flag

    vector<char> m_str_cache; ///< cache the string value

    istream& m_in; ///< input stream
};
#endif
