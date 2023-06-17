#include "errno.h"
#include "lib.h"
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

  if (check_validity(instructions, file_l) !=
      0) { // Are the script loops valid?
    fprintf(stderr, "Instructions contain some invalid loop pattern\n");
    exit(EXIT_FAILURE);
  }

  Context ctx; // Main context
  context_init(&ctx,
               MACHINE_LEN); // Initial status of the context (zero-initialized
                             // machine and head pointer)

  execute(&ctx, instructions, file_l); // Main process

  // Machine state after execution
  printf("Final state of the machine:\n");
  for (long long i = 0; i < MACHINE_LEN; i++) {
    printf("|%d|", ctx.machine[i]);
  }
  printf("\n");

  // Some frees
  free(instructions);
  free(ctx.machine);
  fclose(source_c);
  return EXIT_SUCCESS;
}