#include "lib.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stddef.h>
#include <sys/types.h>
#include "errno.h"


#define MACHINE_LEN 30

int main(int argc, char *argv[]) {
  if (argc < 2) { // source file is needed and machine length
    fprintf(stderr, "Failed getting arguments");
    exit(EXIT_FAILURE);
  }

  FILE *source_c = fopen(argv[1], "r"); // Source file to be interpreted
  if (source_c == NULL) {
    fprintf(stderr, "Failed opening the source file which path was specified");
    exit(EXIT_FAILURE);
  }
  long long file_l = file_len(source_c);
  
  char *instructions = malloc(file_l * sizeof(char)); // The instructions flow (read from file)
  if (instructions == NULL) {
    fprintf(stderr, "Allocation error");
    exit(EXIT_FAILURE);
  }

  if (read_sequence(source_c, instructions, file_l) == -1) { // Read to own iunstructions buffer
    fprintf(stderr, "Failed to read sequence from file");
    exit(EXIT_FAILURE);
  }

  if (check_validity(instructions, file_l) != 0) { // Are the script loops valid?
    fprintf(stderr, "Instructions contain some invalid loop pattern\n");
    exit(EXIT_FAILURE);
  }

  Context ctx; // Main context
  context_init(&ctx, MACHINE_LEN); // Initial status of the context (zero-initialized machine and head pointer)
  
  execute(&ctx, instructions, file_l); // Main process

  // Machine state after execution
  printf("Final state of the machine:\n");
  for (long long i = 0; i < MACHINE_LEN; i++) {
    printf("%d|", ctx.machine[i]);
  }
  printf("\n");


  // Some frees
  free(instructions);
  free(ctx.machine);
  fclose(source_c);
  return EXIT_SUCCESS;
}

int execute(Context *ctx, char *instructions, size_t len) {
  errno = 0;
  for (int instr_idx = 0; instr_idx < len; instr_idx++) {
    switch (instructions[instr_idx]) {
    case '>':
      ctx->pointer = clamp(ctx->pointer+1, 0, MACHINE_LEN-1); // Avoid overflow
      break;
    case '<':
      ctx->pointer = clamp(ctx->pointer-1, 0, MACHINE_LEN); // Avoid overflow
      break;
    case '+':
      *get_pt(ctx) += 1;
      break;
    case '-':
      *get_pt(ctx) -= 1;
      break;
    case ',':
      *get_pt(ctx) = clamp(getc(stdin), 0, 255);
      if (errno != 0) {
        fprintf(stderr, "Clamping issue");
        exit(EXIT_FAILURE);
      }
      break;
    case '.':
      printf("%d\n", *get_pt(ctx));
      break;
    case '[':
      printf("Here is loop, which end is at pos %d", seek(instructions, instr_idx, len, ']'));
      break;
    case ']':
      break;
    }
  }
  return 0;
}

int check_validity(char *instructions, size_t len) { // Loops validity
  int rel = 0;
  for (int i = 0; i < len; i++) {
    char c = instructions[i];
    if (c == '[') {
      rel++;
    } else if (c == ']') {
      rel--;
    }
    if (rel < 0) { // No loop begins with ']'
      return -1;
    }
  }
  if (rel != 0) { // Missed or exceeded
    return -1;
  }
  return 0;
}

void context_init(Context *ctx, long long machine_len) {
  int *machine = (int *)calloc(machine_len, sizeof(int)); // The main machine
  if (machine == NULL) {
    fprintf(stderr, "Allocation error");
    exit(EXIT_FAILURE);
  }
  ctx->machine = machine; // The user defined machine
  ctx->pointer = 0;       // At beginning
}

long long file_len(FILE *fp) {
  fseek(fp, 0, SEEK_END);
  long long len = ftell(fp);
  rewind(fp);
  return len;
}

// Fails if buff is smaller than source code (pointed to by fp)
int read_sequence(FILE *fp, char buff[], size_t buf_len) {
  char c;
  int i = 0;
  if (file_len(fp) < buf_len) {
    return -1;
  }
  while ((c = fgetc(fp)) != EOF) {
    buff[i] = c;
    i++;
  }
  return 0;
}

// Sets the loop -> this traps the instructions bewteen the two brackets
void set_loop(Loop *loop, char *instructions, long from, size_t len) {
  size_t closing_idx =
      seek(instructions, from, len, ']'); // Search for closing in
  if (closing_idx ==
      -1) { // No errors should be yielded, as the flow of instructions was
            // checked beforehand in terms of loops correctness
    fprintf(stderr, "Source code was checked but loops are bugged");
    exit(EXIT_FAILURE);
  }
  loop->looping_instr = substr(instructions, len, from + 1, closing_idx);
  loop->len = closing_idx - from;
}

void handle_loop(char *instructions, size_t len, size_t start, Context *ctx) {
  Loop loop;
  set_loop(&loop, instructions, start, len);
  while (get_pt(ctx) != 0) {
    execute(ctx, loop.looping_instr, loop.len);
  }
  *get_pt(ctx) = (int) start + len + 1;
}

char *substr(char *str, size_t len, size_t idx_pro, size_t idx_post) {
  if (idx_post < idx_pro || idx_post >= len) {
    fprintf(stderr, "Wrong indexes\n");
    exit(EXIT_FAILURE);
  }
  char *buff = malloc((idx_post - idx_pro) * sizeof(char));
  for (int i = idx_pro; i < idx_post; i++) {
    buff[i - idx_pro] = str[i];
  }
  return buff;
}
// Seek for a char in
long seek(char *flow, long beg, size_t len, char pred) {
  for (long i = beg; i < len; i++) {
    if (flow[i] == pred) {
      return i;
    }
  }
  return -1;
}

int get(Context *ctx) {
  return ctx->machine[ctx->pointer];
}

int *get_pt(Context *ctx) { // Returns a pointer to the specific cell (might be mutated then)
  return &ctx->machine[ctx->pointer];
}

int clamp(int value, int min, int max) {
  if (min > max) {
    errno = -1;
  }
  if (value <= min) {
    return min;
  }
  else if (value >= max)
  {
    return max;
  }
  return value;
}
