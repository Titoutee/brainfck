#include "errno.h"
#include "mainlib.h"
#include "optimizer/optimizer.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <stddef.h>
#include <sys/types.h>

#define MACHINE_LEN 30

int main(int argc, char *argv[]) {
  if (argc < 2) { // source file is needed and machine length
    fprintf(stderr, "Failed getting arguments");
    exit(EXIT_FAILURE);
  }

  int optimize = 0;
  if(argc >= 3) {
    char opt_expect[] = "o";
    if (argv[2] == opt_expect) {
      optimize = 1;
    }
  }
  
  FILE *source_c = fopen(argv[1], "r"); // Source file to be interpreted
  if (source_c == NULL) {
    fprintf(stderr, "Failed opening the source file which path was specified");
    exit(EXIT_FAILURE);
  }
  long long file_l = file_len(source_c);

  char *instructions =
      malloc(file_l * sizeof(char)); // The instructions flow (read from file)
  if (instructions == NULL) {
    fprintf(stderr, "Allocation error");
    exit(EXIT_FAILURE);
  }

  if (read_sequence(source_c, instructions, file_l) ==
      -1) { // Read to own iunstructions buffer
    fprintf(stderr, "Failed to read sequence from file");
    exit(EXIT_FAILURE);
  }

  remove_char_all(instructions, ' ', strlen(instructions) + 1);
  remove_char_all(instructions, '\n', strlen(instructions) + 1);
  
  if (optimize) {
    printf("yay let's optimize your code...\n");
    optimizer(instructions);
    printf("Done!\n");
  }

  if (check_validity(instructions, file_l) !=
      0) { // Are the script loops valid?
    fprintf(stderr, "Instructions contain some invalid loop pattern\n");
    exit(EXIT_FAILURE);
  }

  //optimizer(instructions);
  //printf("Final instrucitons: %s", instructions);
  Context ctx; // Main context
  context_init(&ctx,
               MACHINE_LEN); // Initial status of the context (zero-initialized
                             // machine and head pointer)

  execute(&ctx, instructions, file_l); // Main process

  // Machine state after execution
  printf("Final state of the machine:\n");
  for (long long i = 0; i < MACHINE_LEN; i++) {
    printf("%d|", ctx.machine[i]);
  }
  printf("\n");
  printf("With chars:\n");
  for (long long i = 0; i < MACHINE_LEN; i++) {
    printf("%c|", ctx.machine[i]);
  }

  // Some frees
  free(instructions);
  free(ctx.machine);
  fclose(source_c);
  return EXIT_SUCCESS;
}