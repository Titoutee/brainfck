#include "../src/mainlib.h"
#include "../src/optimizer/optimizer.h"
#include "stdlib.h"
#include "string.h"
#include <stdio.h>


int main(void) {
    char string[] = "Heya";
    remove_char(string, 'H', 4);
    insert_at(string, 'u', 0);
    //int res = insert_at(string, 'i', 4);
    //if (res == -1) {
    //    fprintf(stderr, "Error while inserting char in string");
    //    exit(EXIT_FAILURE);
    //}
    printf("%s\n", string);
    return 0;
}
