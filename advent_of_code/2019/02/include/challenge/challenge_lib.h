/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-02
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stdlib.h"

#define INSTRUCTION_SIZE (size_t) 4


typedef enum
{
    INT_CODE_ERROR    = 0,
    INT_CODE_HALT     = 1,
    INT_CODE_CONTINUE = 2,
} intcode_ret_t;

typedef enum
{
    OP_CODE_ADD  = 1,
    OP_CODE_MULT = 2,
    OP_CODE_HALT = 99,
} intcode_op_codes_t;

typedef struct
{
    int* memory;
    size_t memory_size;
    size_t head;
} intcode_t;

intcode_t* read_intcode(const char* const file_path);
intcode_t* create_intcode(int* const content, const size_t content_size);
void destroy_intcode(intcode_t* const prog);
void print_intcode(const intcode_t* const prog);
intcode_t* copy_intcode(const intcode_t* const prog);
int output_intcode(const intcode_t* const prog);

int execute(intcode_t* const prog);
int execute_head_block(intcode_t* const prog);
void move_head(intcode_t* const prog);

void add_op(intcode_t* const prog, const int first, const int second, const int result);
void multiply_op(intcode_t* const prog, const int first, const int second, const int result);


#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
