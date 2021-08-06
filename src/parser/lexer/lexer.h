#ifndef LEXER_H
#define LEXER_H 1

#include "token_chain.h"

TokenChain* tokenize_buildfile(const char* path);

#endif