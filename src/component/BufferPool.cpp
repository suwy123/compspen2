/*******************************************
*  @file  BufferPool.cpp                   * 
*  @brief    Brief file description        *
*                                          *
*  @author   Chong Gao                     *
*  @version  1.0                           *
*  @date     2018-12-3                     *
*                                          *
*******************************************/

#include "component/TokenBuffer.h"
#include "component/TokenScannerBuffer.h"
#include "component/CommandParserBuffer.h"
#include "component/Z3Buffer.h"
#include "TokenScannerFactory.h"
#include "CommandParserFactory.h"
#include "Types.h"

z3::context z3_ctx;
TokenScannerFactory tokenScannerFactory;
CommandParserFactory cmdParserFactory;
TokenBuffer token_buffer;
TokenScannerBuffer token_scanner_buffer;
CommandParserBuffer cmd_parser_buffer;
Z3Buffer z3_buffer;

// error info
SyntaxErrorTable SYNTAX_ERROR_INFO = {
    "",
    "'(' is expected!",
    "')' is expected!",
    "keyword is expected!",
    "symbol is expected!",
    "string literal is expected!",
    "int literal is expected!",
    "float literal is expected!",
    "bit vector is expected!", 
    "EOF is expected!"
};

