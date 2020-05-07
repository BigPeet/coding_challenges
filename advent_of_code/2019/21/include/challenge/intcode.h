/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-05-07
 *
 */

#ifndef INCLUDE_INTCODE_H
#define INCLUDE_INTCODE_H

#include "stdlib.h"
#include "stdio.h"
#include "pthread.h"

typedef enum
{
    INT_CODE_ERROR    = 0,
    INT_CODE_HALT     = 1,
    INT_CODE_CONTINUE = 2,
} intcode_ret_t;

typedef enum
{
    INT_CODE_STD_IO = 0,
    INT_CODE_MEM_IO = 1,
} intcode_io_mode_t;

typedef struct
{
    int64_t value;
    int consumed;
    pthread_mutex_t mut;
    pthread_cond_t cond;
} intcode_io_mem_t;

typedef struct
{
    int64_t* memory;
    size_t memory_size;
    size_t head;
    int64_t relative_base;
    intcode_io_mode_t io_mode;
    intcode_io_mem_t* mem_io_in;
    intcode_io_mem_t* mem_io_out;
    FILE* std_io_in;
    FILE* std_io_out;
    int waiting_for_input;
} intcode_t;

intcode_t* read_intcode(const char* const file_path);
intcode_t* create_intcode(int64_t* const content, const size_t content_size);
void destroy_intcode(intcode_t* const prog);
void print_intcode(const intcode_t* const prog);
int set_mem_value(intcode_t* const prog, const size_t address, const int64_t value);
int64_t get_mem_value(const intcode_t* const prog, const size_t address);
void set_io_mode(intcode_t* const prog, const intcode_io_mode_t mode);
void set_mem_io_in(intcode_t* const prog, intcode_io_mem_t* const input_store);
void set_mem_io_out(intcode_t* const prog, intcode_io_mem_t* const output_store);
void set_std_io_in(intcode_t* const prog, FILE* const input_stream);
void set_std_io_out(intcode_t* const prog, FILE* const output_stream);
intcode_t* copy_intcode(const intcode_t* const prog);
int output_intcode(const intcode_t* const prog);
int waiting_for_input(const intcode_t* const prog);

intcode_io_mem_t* create_io_mem();
void destroy_io_mem(intcode_io_mem_t* const store);

int execute(intcode_t* const prog);
int execute_head_block(intcode_t* const prog, int* const op_code);

int add_op(intcode_t* const prog, const int64_t* const parameters);
int multiply_op(intcode_t* const prog, const int64_t* const parameters);
int input_op(intcode_t* const prog, const int64_t* const parameters);
int output_op(intcode_t* const prog, const int64_t* const parameters);
int jmp_if_true_op(intcode_t* const prog, const int64_t* const parameters);
int jmp_if_false_op(intcode_t* const prog, const int64_t* const parameters);
int is_less_op(intcode_t* const prog, const int64_t* const parameters);
int is_equals_op(intcode_t* const prog, const int64_t* const parameters);
int adjust_rel_base_op(intcode_t* const prog, const int64_t* const parameters);
int error_op(intcode_t* const prog, const int64_t* const parameters);


#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
