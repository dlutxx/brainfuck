#include "brainfuck.h"
#include <stdio.h>
#include <assert.h>


int main(int argc, char* argv[])
{
    FILE* fp = fopen(argv[1], "r");
    BFState* st = bf_create_state(DEFAULT_BF_SLOTS);
    BFCode* code= bf_compile(fp);
    assert(st != NULL);
    assert(NULL != code);

    return bf_execute(code, st);
}
