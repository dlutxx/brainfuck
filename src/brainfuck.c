#include "brainfuck.h"
#include <stdlib.h>

BFState* bf_create_state(unsigned long size)
{
    BFState * st;
    st = calloc(1, size + sizeof(BFState));
    st->slot_size = size;
    return st;
}

int bf_free_state(BFState* stat)
{
    if (NULL!=stat)
        free(stat);
    return 0;
}

static BFInstruction* bf_create_instruction(char type)
{
    BFInstruction *i = malloc(sizeof(BFInstruction));
    if (NULL == i) {
        fprintf(stderr, "Out of Memory\n");
        exit(1);
    }
    i->type = type;
    i->repeat = 1;
    i->next = i->child = NULL;
    return i;
}

static BFInstruction* bf_compile(FILE* fp)
{
    BFInstruction *root=NULL, *tail=NULL, *i;
    int chr;
    while ((chr=fgetc(fp))!=EOF) {
        if (BF_TOKEN_PLUS == chr || BF_TOKEN_MINUS == chr ||
            BF_TOKEN_NEXT == chr || BF_TOKEN_PREVIOUS == chr ||
            BF_TOKEN_INPUT == chr || BF_TOKEN_OUTPUT == chr) {
            if (tail !=NULL && tail->type == chr) {
                i = tail;
                ++ i->repeat;
            } else {
                i = bf_create_instruction(chr);
                if (tail == NULL)
                    root = tail = i;
                else {
                    tail->next = i;
                    tail = i;
                }
            }
            while (i->type == (chr=fgetc(fp)))
                ++ i->repeat;
            ungetc(chr, fp);
        } else if (BF_TOKEN_LOOP_START == chr) {
            i = bf_create_instruction(chr);
            if (tail == NULL)
                root = tail = i;
            else {
                tail->next = i;
                tail = i;
            }
            i->child = bf_compile(fp);
        } else if (BF_TOKEN_LOOP_END == chr) {
            break;
        } else {
            // ignore unknown chars
        }
    }
    return root;
}

BFCode* bf_compile_file(FILE* fp)
{
    BFCode * code;
    code = malloc(sizeof(BFCode));
    if (NULL==code) {
        fprintf(stderr, "Out of Memory");
        exit(1);
    }
    code->head = bf_compile(fp);
    return code;
}

BFCode* bf_compile_str(char* str, size_t len)
{
    FILE* mfp = fmemopen(str, len, "r");
    return bf_compile_file(mfp);
}

static BFInstruction* _execute(BFCode* code, BFInstruction* exe, BFState* st)
{
    // int brkt=0;
    while (NULL != exe) {
        // fprintf(stdout, "%c, %d\n", exe->type, exe->repeat);
        if (BF_TOKEN_NEXT == exe->type) {
            st->slot_index += exe->repeat;
            if (st->slot_index >= st->slot_size) {
                st->error = ERR_SLOTS_OVERFLOW;
                    fprintf(stderr, "%d %lu %zu\n", __LINE__, st->slot_index, st->slot_size);
                    exit(1);
                break;
            }
        } else if (BF_TOKEN_PREVIOUS == exe->type) {
            if (st->slot_index == 0) {
                st->error = ERR_SLOTS_UNDERFLOW;
                    fprintf(stderr, "%d\n", __LINE__);
                    exit(1);
                break;
            }
            st->slot_index -= exe->repeat;
        } else if (BF_TOKEN_PLUS == exe->type) {
            st->slots[st->slot_index] += exe->repeat;
        } else if (BF_TOKEN_MINUS == exe->type) {
            st->slots[st->slot_index] -= exe->repeat;
        } else if (BF_TOKEN_INPUT == exe->type) {
            int n;
            for (n=0; n<exe->repeat; ++n)
                st->slots[st->slot_index] = (char)getchar();
        } else if (BF_TOKEN_OUTPUT == exe->type) {
            int n;
            for (n=0; n<exe->repeat; ++n)
                putchar(st->slots[st->slot_index]);
        } else if (BF_TOKEN_LOOP_START == exe->type) {
            while (st->slots[st->slot_index])
                _execute(code, exe->child, st);
        } else {
            fprintf(stderr, "Wtf %c?\n", exe->type);
            exit(1);
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

int bf_dump_code(BFInstruction* i)
{
    while (NULL != i) {
        if (BF_TOKEN_LOOP_START == i->type) {
            printf("%c ", i->type);
            bf_dump_code(i->child);
            printf(" %c", BF_TOKEN_LOOP_END);
        } else {
            printf("%c%d, ", i->type, i->repeat);
        }
        i = i->next;
    }
    return 0;
}

int bf_free_code(BFCode* code)
{
    return 0;
}
