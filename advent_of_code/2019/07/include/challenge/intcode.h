/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-05
 *
 */

#ifndef INCLUDE_INTCODE_H
#define INCLUDE_INTCODE_H

#include "stdlib.h"

typedef enum
{
    INT_CODE_ERROR    = 0,
    INT_CODE_HALT     = 1,
    INT_CODE_CONTINUE = 2,
} intcode_ret_t;

typedef enum
{
    OP_CODE_ADD          = 1,
    OP_CODE_MULT         = 2,
    OP_CODE_INPUT        = 3,
    OP_CODE_OUTPUT       = 4,
    OP_CODE_JMP_IF_TRUE  = 5,
    OP_CODE_JMP_IF_FALSE = 6,
    OP_CODE_IS_LESS      = 7,
    OP_CODE_IS_EQUALS    = 8,
    OP_CODE_HALT         = 99,
} intcode_op_codes_t;

typedef enum
{
    PARAM_MODE_POSITION  = 0,
    PARAM_MODE_IMMEDIATE = 1,
} intcode_param_modes_t;

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
int execute_head_block(intcode_t* const prog, int* const op_code);

int add_op(intcode_t* const prog, const int* const parameters);
int multiply_op(intcode_t* const prog, const int* const parameters);
int input_op(intcode_t* const prog, const int* const parameters);
int output_op(intcode_t* const prog, const int* const parameters);
int jmp_if_true_op(intcode_t* const prog, const int* const parameters);
int jmp_if_false_op(intcode_t* const prog, const int* const parameters);
int is_less_op(intcode_t* const prog, const int* const parameters);
int is_equals_op(intcode_t* const prog, const int* const parameters);
int error_op(intcode_t* const prog, const int* const parameters);


#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */

