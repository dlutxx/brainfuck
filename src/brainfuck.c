#include "brainfuck.h"
#include <stdlib.h>

BFState* bf_create_state(unsigned long size)
{
    BFState * st;
    st = calloc(size + sizeof(BFState), 1);
    st->slot_size = size;
    return st;
}

int bf_free_state(BFState* stat)
{
    if (NULL!=stat)
        free(stat);
    return 0;
}


BFCode* bf_compile_file(FILE* fp)
{
    BFCode * code = calloc(sizeof(BFCode), 1);
    int chr, brackets=0;
    if (NULL!=code) {
        while ((chr=fgetc(fp))!=EOF) {
            ++code->errpos;
            if (BF_TOKEN_PLUS != chr && BF_TOKEN_MINUS != chr &&
                BF_TOKEN_NEXT != chr && BF_TOKEN_PREVIOUS != chr &&
                BF_TOKEN_INPUT != chr && BF_TOKEN_OUTPUT != chr &&
                BF_TOKEN_LOOP_START != chr && BF_TOKEN_LOOP_END != chr)
                continue; // skip illegal chars

            if (BF_TOKEN_LOOP_START == chr)
                ++brackets;
            else if (BF_TOKEN_LOOP_END == chr && --brackets<0) {
                code->error = ERR_UNBALANCED_BRACKET; 
                return code;
            }
            BFInstruction * i = calloc(sizeof(BFInstruction), 1);
            if (i==NULL) {
                code->error = ERR_MEMORY;
                return code;
            }
            i->type = chr;
            if (code->tail == NULL)
                code->head = code->tail = i;
            else {
                code->tail->next = i;
                code->tail = i;
            }
        }
        if (brackets)
            code->error = ERR_UNBALANCED_BRACKET;
        if (ferror(fp))
            code->error = ERR_READ;
        fclose(fp);
    }
    return code;
}

BFCode* bf_compile_str(char* str, size_t len)
{
    FILE* mfp = fmemopen(str, len, "r");
    return bf_compile_file(mfp);
}

static BFInstruction* _execute(BFCode* code, BFInstruction* exe, BFState* st)
{
    BFInstruction* i;
    while (NULL != exe) {
        // fprintf(stderr, "%c", exe->type);
        if (BF_TOKEN_NEXT == exe->type) {
            ++st->slot_index;
            if (st->slot_index >= st->slot_size) {
                st->error = ERR_SLOTS_OVERFLOW;
                break;
            }
        } else if (BF_TOKEN_PREVIOUS == exe->type) {
            if (st->slot_index == 0) {
                st->error = ERR_SLOTS_UNDERFLOW;
                break;
            }
            --st->slot_index;
        } else if (BF_TOKEN_PLUS == exe->type) {
            ++st->slots[st->slot_index];
        } else if (BF_TOKEN_MINUS == exe->type) {
            --st->slots[st->slot_index];
        } else if (BF_TOKEN_INPUT == exe->type) {
            st->slots[st->slot_index] = (char)getchar();
        } else if (BF_TOKEN_OUTPUT == exe->type) {
            putchar(st->slots[st->slot_index]);
        } else if (BF_TOKEN_LOOP_START == exe->type) {
            while (st->slots[st->slot_index])
                i = _execute(code, exe->next, st); // Expecting i = LOOP_END
            if (NULL == i) {
                st->error = ERR_SYNTAX_UNEXPECTED_END;
                break;
            }
            if (BF_TOKEN_LOOP_END != i->type) {
                st->error = ERR_SYNTAX_UNEXPECTED_TOKEN;
                break;
            }
            exe = i;
        } else if (BF_TOKEN_LOOP_END == exe->type) {
            return exe;
        } else {
            st->error = ERR_SYNTAX_ILLEGAL_TOKEN;
            return NULL;
        }
        exe = exe->next;
    }
    return NULL;
}

int bf_execute(BFCode* code, BFState* st)
{
    if (NULL!=_execute(code, code->head, st))
        return 1;
    if (st->error) {
        fprintf(stderr, "execute error: %d\n", st->error);
        return 1;
    }
    return 0;
}

int bf_dump_code(BFCode* code)
{
    BFInstruction * i;
    i = code->head;
    while (NULL != i) {
        putchar(i->type);
        i = i->next;
    }
    return 0;
}

int bf_free_code(BFCode* code)
{
    return 0;
}
