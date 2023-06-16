#include "stdio.h"
#include "stdlib.h"
#include <stddef.h>


typedef struct {
    long long pointer;
    int *machine;
} Context;

/// @brief Gets the 8-bit value at the current `pointer` position in the current machine
/// @param ctx The context in question
/// @return The value pointed by the internal pointer
int get(Context *ctx);

/// @brief Gets a reference to the 8-bit value at the current `pointer` position in the current machine
/// @param ctx The context in question
/// @return A reference to the value pointed by the internal pointer (might be mutated thus)
int *get_pt(Context *ctx);


typedef struct {
    char *looping_instr;
    size_t len;
} Loop;


void set_loop(Loop *loop, char *instructions, long from, size_t len);


/// @brief Initialise the main context (machine + pointer)
/// @param ctx The context to initialize
/// @param machine_len The length of the internal machine
void context_init(Context *ctx, long long machine_len);

/// @brief Processes the flow of instructions
/// @param ctx The context, which may vary during the program execution
/// @param instructions The flow of instructions to follow
/// @param len The length of the flow
/// @return 
int execute(Context *ctx, char *instructions, size_t len);

/// @brief Checks the validity  of the instructions flow (in terms of loop(s) arrangement)
/// @param instructions The intructions flow
/// @param len The length of the flow
/// @return 
int check_validity(char *instructions, size_t len);// Check loop validity

/// @brief For handling loops (capture inbetween instructions and return to flow after execution)
/// @param instructions The flow of instructions
/// @param len The length of the flow
/// @param start The starting point of the loop
/// @param ctx The context
void handle_loop(char *instructions, size_t len, size_t start, Context *ctx);


//Utils
long seek(char *flow, long beg, size_t len, char pred);
long long file_len(FILE* fp); // Gets file_len (util function)
int read_sequence(FILE *fp, char buff[], size_t buf_len); // Stores the flow of instructions into tokens into buff
char *substr(char *str, size_t len, size_t idx_pro, size_t idx_post);
int clamp(int value, int min, int max);
//long long relative_offset_count(char *instructions, size_t machine_len);