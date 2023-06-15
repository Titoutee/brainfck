#include "stdio.h"
#include "stdlib.h"
#include <stddef.h>

typedef struct {
    long long pointer;
    int *machine;
} Context;

typedef struct {
    char *looping_instr;
    size_t len;
} Loop;


void context_init(Context *ctx, long long machine_len); // Initialize a context
int execute(Context *ctx, char *instructions, size_t len); // Processes the flow of instructions (core)
int check_validity(char *instructions, size_t len);// Check loop validity
void set_loop(Loop *loop, char *instructions, long from, size_t len);
void input(Context *ctx);
void handle_loop(char *instructions, size_t len, size_t start, Context *ctx);
//Utils
long seek(char *flow, long beg, size_t len, char pred);
long long file_len(FILE* fp); // Gets file_len (util function)
int read_sequence(FILE *fp, char buff[], size_t buf_len); // Stores the flow of instructions into tokens into buff
char *substr(char *str, size_t len, size_t idx_pro, size_t idx_post);