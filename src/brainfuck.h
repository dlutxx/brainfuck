#ifndef BRAINFUCK_H
#define BRAINFUCK_H

#include <stdio.h>

// default slot size for BFState
#define DEFAULT_BF_SLOTS 4096

#define BF_TOKEN_NEXT '>'
#define BF_TOKEN_PREVIOUS '<'
#define BF_TOKEN_PLUS '+'
#define BF_TOKEN_MINUS '-'
#define BF_TOKEN_OUTPUT '.'
#define BF_TOKEN_INPUT ','
#define BF_TOKEN_LOOP_START '['
#define BF_TOKEN_LOOP_END ']'

#define ERR_READ 1
#define ERR_UNBALANCED_BRACKET 2
#define ERR_MEMORY 3
#define ERR_SLOTS_OVERFLOW 4
#define ERR_SLOTS_UNDERFLOW 5
#define ERR_SYNTAX_UNEXPECTED_END 6
#define ERR_SYNTAX_UNEXPECTED_TOKEN 7
#define ERR_SYNTAX_ILLEGAL_TOKEN 8

typedef struct BFInstruction {
    struct BFInstruction *next;
    struct BFInstruction *child; // if this is a while loop, this points to sub-instructions;
    int repeat; // how many consecutive identical instructions
    char type;
} BFInstruction;

typedef struct BFCode {
    BFInstruction *head;
    BFInstruction *tail;
    size_t errpos;
    int error;
} BFCode;

typedef struct BFState {
    unsigned long slot_size;
    unsigned long slot_index;
    int error;
    char slots[];
} BFState;

BFState* bf_create_state(unsigned long size);
int bf_free_state(BFState* stat);
BFCode* bf_compile_file(FILE* fp);
BFCode* bf_compile_str(char* str, size_t len);
int bf_execute(BFCode* code, BFState* st);
int bf_dump_code(BFInstruction* code);
int bf_free_code(BFCode* code);

#endif
