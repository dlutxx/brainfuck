#include "brainfuck.h"
#include <stdio.h>
#include <assert.h>


int main(int argc, char* argv[])
{
    FILE* fp = fopen(argv[1], "r");
    int c;
    BFState* st = bf_create_state(DEFAULT_BF_SLOTS);
    BFCode* code= bf_compile_file(fp);
    assert(st != NULL);
    assert(NULL != code);

    // bf_dump_code(code->head);
    c = bf_execute(code, st);
    printf("\n");
    return c;
}
