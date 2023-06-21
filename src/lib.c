#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include "mainlib.h"
#include "optimizer/optimizer.h"
#include <stddef.h>
#include <string.h>

#define MACHINE_LEN 30

void execute(Context *ctx, char *instructions, size_t len) {
  errno = 0;
  long step = 0;
  while (step < len) {
    switch (instructions[step]) {
    case '>':
      ctx->pointer =
          clamp(ctx->pointer + 1, 0, MACHINE_LEN - 1); // Avoid machine overflow
      break;
    case '<':
      ctx->pointer =
          clamp(ctx->pointer - 1, 0, MACHINE_LEN); // Avoid machine overflow
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
      printf("%c", *get_pt(ctx));
      break;
    case '[':
      step = handle_loop(
          instructions, len, step /*The opening bracket we are on*/,
          ctx); // the execution process let handle_loop the responsability
      break;
    }
    step++;
  }
}

int check_validity(char *instructions, size_t len) { // Loops validity
  int total_count = total_count_relative(instructions, '[', ']', len);
  if (total_count != 0) { // So if -1 (ill-ordered brackets) or exceeded brackets (opening or closing)
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

long set_loop(Loop *loop, char instructions[], long start, size_t len) {
  long closing_bracket_idx = seek(instructions, start, len, ']'); // -1 if error
  if (closing_bracket_idx == -1 || closing_bracket_idx == start + 1) {
    fprintf(stderr,
            "Unrecoverable error: checked for correct loop pattern beforehand "
            "but an error occured while attempting to close loop...\n");
    exit(EXIT_FAILURE);
  }
  loop->looping_instr =
      substr(instructions, len, start + 1, closing_bracket_idx-1);
  loop->len = closing_bracket_idx - start - 1;
  printf("Got a loop: instruction length: %ld, and instructions: %s\n", loop->len, loop->looping_instr);
  return closing_bracket_idx;
}

long handle_loop(char instructions[], size_t len, size_t start, Context *ctx) {
  Loop loop;
  long end_loop = set_loop(&loop, instructions, start, len);
  while (get(ctx) != 0) {
    execute(ctx, loop.looping_instr, loop.len); // Loop approach is recursive
  }
  return end_loop;
}

char *substr(char str[], size_t len, size_t idx_pro, size_t idx_post) {
  if (idx_post < idx_pro || idx_post >= len) {
    fprintf(stderr, "Wrong indexes\n");
    exit(EXIT_FAILURE);
  }
  char *buff = malloc((idx_post - idx_pro) * sizeof(char));

  for (int i = idx_pro; i <= idx_post; i++) { // idx_post is included
    buff[i - idx_pro] = str[i];
  }
  return buff;
}
// Seek for a predicate in a string starting at some index
long seek(char *flow, long beg, size_t len, char pred) {
  for (int i = beg+1; i < len; i++) {
    if (check_validity(substr(flow, len, beg, i), i-beg+1) == 0) { // If the slice is correct loop speaking (support for nested loops then)
      return i;
    }
  }
  return -1;
}

long total_count_relative(char *str, char pred_plus, char pred_minus, size_t len) {
  int glob_c = 0;
  for (int i = 0; i<len; i++) {
    if (str[i] == pred_plus) {
      glob_c++;
    }  else if (str[i] == pred_minus) {
      glob_c--;
    }
    if (glob_c<0) {
      return -1;
    }
  }
  return glob_c;
}

int get(Context *ctx) { return ctx->machine[ctx->pointer]; }

int *get_pt(Context *ctx) { // Returns a pointer to the specific cell (might be
                            // mutated then)
  return &ctx->machine[ctx->pointer];
}

int clamp(int value, int min, int max) {
  if (min > max) {
    errno = -1;
  }
  if (value <= min) {
    return min;
  } else if (value >= max) {
    return max;
  }
  return value;
}

void remove_char(char string[], char pred, size_t len) {
    int i = 0;
    while (i < len) {
        if (string[i] == pred) {
            remove_char_at(string, len, i);
            break;
        } else i++;
    }
}

void remove_char_all(char string[], char pred, size_t len) {
    int i = 0;
    while (i < len) {
        if (string[i] == pred) {
            remove_char_at(string, len, i);
            //break;
        } else i++;
    }
}

void remove_char_at(char string[], size_t len, size_t idx) {
  if (idx>= len) {
    fprintf(stderr, "Bad index");
    exit(EXIT_FAILURE);
  }
  for (int i = idx; i < len; i++) {
    string[i] = string[i+1];
  }
}

// DANGER: Do NOT use
int insert_at(char string[], char insert, size_t idx) {
  if (idx > strlen(string)) { // Or i > strlen(string) + 1, as it doesn't count \0
    return -1;
  }
  int i;
  string = realloc(string, sizeof(char) * (strlen(string) + 2)); // We reallocate space for one more element
  if (string == NULL) {
    fprintf(stderr, "Reallocation failed");
    exit(EXIT_FAILURE);
  }
  for (i = strlen(string)-2; i > idx; i--) {
    string[i+1] = string[i];
  }
  string[i] = insert;
  return 0;
}

void optimizer(char instructions_flow[])  {
    remove_char_all(instructions_flow, ' ', strlen(instructions_flow) + 1);
}

//int segment_count(char *trunc_from_beg, char pred_plus, char pred_minus) {
//  int c = 0;
//  for (int i = 0; i < strlen(trunc_from_beg); i++) {
//    
//  }
//}