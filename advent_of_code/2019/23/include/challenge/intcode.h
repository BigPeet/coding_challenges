/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-05-07
 *
 */

#ifndef INCLUDE_INTCODE_H
#define INCLUDE_INTCODE_H

#include "pthread.h"
#include "stdio.h"
#include "stdlib.h"

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

intcode_t* read_intcode(const char* file_path);
intcode_t* create_intcode(int64_t* memory, size_t memory_size);
void destroy_intcode(intcode_t* prog);
void print_intcode(const intcode_t* prog);
int set_mem_value(intcode_t* prog, size_t address, int64_t value);
int64_t get_mem_value(const intcode_t* prog, size_t address);
void set_io_mode(intcode_t* prog, intcode_io_mode_t mode);
void set_mem_io_in(intcode_t* prog, intcode_io_mem_t* input_store);
void set_mem_io_out(intcode_t* prog, intcode_io_mem_t* output_store);
void set_std_io_in(intcode_t* prog, FILE* input_stream);
void set_std_io_out(intcode_t* prog, FILE* output_stream);
intcode_t* copy_intcode(const intcode_t* prog);
int output_intcode(const intcode_t* prog);
int waiting_for_input(const intcode_t* prog);

intcode_io_mem_t* create_io_mem();
void destroy_io_mem(intcode_io_mem_t* store);

int execute(intcode_t* prog);
int execute_head_block(intcode_t* prog, int* op_code);

int add_op(intcode_t* prog, const int64_t* parameters);
int multiply_op(intcode_t* prog, const int64_t* parameters);
int input_op(intcode_t* prog, const int64_t* parameters);
int output_op(intcode_t* prog, const int64_t* parameters);
int jmp_if_true_op(intcode_t* prog, const int64_t* parameters);
int jmp_if_false_op(intcode_t* prog, const int64_t* parameters);
int is_less_op(intcode_t* prog, const int64_t* parameters);
int is_equals_op(intcode_t* prog, const int64_t* parameters);
int adjust_rel_base_op(intcode_t* prog, const int64_t* parameters);
int error_op(intcode_t* prog, const int64_t* parameters);


#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
