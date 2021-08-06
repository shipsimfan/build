#include "token_chain.h"

#include <stdio.h>
#include <stdlib.h>

TokenChain* create_token_chain() {
    TokenChain* token_chain = malloc(sizeof(TokenChain));
    token_chain->head = NULL;
    token_chain->tail = NULL;
    return token_chain;
}

void push_token_chain(TokenChain* token_chain, Token* token) {
    if (token_chain == NULL || token == NULL)
        return;

    TokenNode* new_node = malloc(sizeof(TokenNode));
    new_node->token = token;
    new_node->next = NULL;

    if (token_chain->head == NULL) {
        token_chain->head = new_node;
        token_chain->tail = new_node;
    } else {
        token_chain->tail->next = new_node;
        token_chain->tail = new_node;
    }
}

void push_front_token_chain(TokenChain* token_chain, Token* token) {
    if (token_chain == NULL || token == NULL)
        return;

    TokenNode* new_node = malloc(sizeof(TokenNode));
    new_node->token = token;
    new_node->next = token_chain->head;

    if (token_chain->head == NULL)
        token_chain->tail = new_node;
    token_chain->head = new_node;
}

Token* pop_token_chain(TokenChain* token_chain) {
    if (token_chain->head == NULL)
        return NULL;

    TokenNode* node = token_chain->head;
    token_chain->head = node->next;
    if (token_chain->head == NULL)
        token_chain->tail = NULL;

    Token* token = node->token;
    free(node);

    return token;
}

void display_token_chain(TokenChain* token_chain) {
    if (token_chain == NULL)
        return;

    printf("Tokens\n");
    printf("------------------------------\n");
    TokenNode* node = token_chain->head;
    while (node != NULL) {
        display_token(node->token, stdout);
        putchar('\n');
        node = node->next;
    }

    putchar('\n');
}

void destroy_token_chain(TokenChain* token_chain) {
    if (token_chain == NULL)
        return;

    TokenNode* node = token_chain->head;
    while (node != NULL) {
        TokenNode* next = node->next;
        destroy_token(node->token);
        free(node);
        node = next;
    }

    free(token_chain);
}