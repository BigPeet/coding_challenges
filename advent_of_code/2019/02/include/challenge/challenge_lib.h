#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stdlib.h"

typedef struct
{
    int* content;
    size_t content_size;
    size_t head;
} intcode_t;

intcode_t* read_intcode(const char* const file_path);

intcode_t* create_intcode(int* const content, const size_t content_size);
void destroy_intcode(intcode_t* const prog);

int execute(intcode_t* const prog);
int execute_head_block(intcode_t* const prog);
int move_head(intcode_t* const prog);

void add_op(intcode_t* const prog, const int first, const int second, const int result);
void multiply_op(intcode_t* const prog, const int first, const int second, const int result);


#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
