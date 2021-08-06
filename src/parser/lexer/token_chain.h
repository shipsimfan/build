#ifndef TOKEN_CHAIN_H
#define TOKEN_CHAIN_H 1

#include "token.h"

typedef struct TokenNode_t {
    Token* token;
    struct TokenNode_t* next;
} TokenNode;

typedef struct {
    TokenNode* head;
    TokenNode* tail;
} TokenChain;

TokenChain* create_token_chain();
void push_token_chain(TokenChain* token_chain, Token* token);
void push_front_token_chain(TokenChain* token_chain, Token* token);
Token* pop_token_chain(TokenChain* token_chain);
void display_token_chain(TokenChain* token_chain);
void destroy_token_chain(TokenChain* token_chain);

#endif