#include "stdio.h"
#include <stddef.h>
#include <string.h>

/// This library provides a basic API for brainfuck optimization operations

///Base function for removing a char from a string (it must not be \0)
void remove_char(char string[], char pred, size_t len);
void remove_char_all(char string[], char pred, size_t len);
void remove_char_at(char string[], size_t len, size_t idx);
int insert_at(char string[], char insert, size_t idx);


/// Main optimizing process function.
/// It removes both redundancy problems and exec time excessing code fragments
void optimizer(char instructions_flow[]);

