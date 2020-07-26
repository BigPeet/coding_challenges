/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-05
 *
 */

#include "challenge/intcode.h"
#include "string.h"

#define INTCODE_DELIM ","
#define INTCODE_NO_STORE -1
/*#define DEBUG 1*/

typedef enum
{
    OP_CODE_ADD             = 1,
    OP_CODE_MULT            = 2,
    OP_CODE_INPUT           = 3,
    OP_CODE_OUTPUT          = 4,
    OP_CODE_JMP_IF_TRUE     = 5,
    OP_CODE_JMP_IF_FALSE    = 6,
    OP_CODE_IS_LESS         = 7,
    OP_CODE_IS_EQUALS       = 8,
    OP_CODE_ADJUST_REL_BASE = 9,
    OP_CODE_HALT            = 99,
} intcode_op_codes_t;

typedef enum
{
    PARAM_MODE_POSITION  = 0,
    PARAM_MODE_IMMEDIATE = 1,
    PARAM_MODE_RELATIVE  = 2,
} intcode_param_modes_t;

typedef int (*intcode_op_f)(intcode_t* const, const int64_t* const);

static void get_size_info(const char* const file_path,
                          size_t* const total_chars,
                          size_t* const amount_integers);

static size_t get_instruction_size(const int op_code);
static int get_opcode(const int64_t number);
static int is_valid_opcode(const int op_code);

static intcode_op_f get_op_func(const int op_code);
static void get_parameter_modes(const int64_t number,
                                const size_t num_parameters,
                                int* const parameter_modes);

static int get_parameter_values(const intcode_t* const prog,
                                const size_t num_parameters,
                                const int store_param,
                                const int* const parameter_modes,
                                int64_t* const parameters);

static void write_to_io_std(FILE* const stream, const int64_t value);
static int read_from_io_std(FILE* const stream, int64_t* value);
static void write_to_io_mem(intcode_io_mem_t* const storage, const int64_t value);
static void read_from_io_mem(intcode_io_mem_t* const storage, int64_t* value);


intcode_t* read_intcode(const char* const file_path)
{
    intcode_t* prog = NULL;
    if (file_path != NULL)
    {
        size_t num_chars = 0;
        size_t num_ints  = 0;
        get_size_info(file_path, &num_chars, &num_ints);

        char* str       = (char*) malloc(sizeof(char) * num_chars);
        int64_t* memory = (int64_t*) malloc(sizeof(int64_t) * num_ints);
        FILE* fp        = fopen(file_path, "r");
        if ((fp != NULL) && (str != NULL) && (memory != NULL))
        {
            /*We only read the first line.*/
            if (fscanf(fp, "%s", str) == EOF)
            {
                printf("No lines in file.\n");
            }
            fclose(fp);

            /*Tokenize string*/
            size_t index = 0;
            char* token  = strtok(str, INTCODE_DELIM);
            while (token != NULL)
            {
                /*TODO might need changing to correctly parse int64_t*/
                memory[index++] = strtoll(token, NULL, 10);
                token           = strtok(NULL, INTCODE_DELIM);
            }
            free(str);

            prog = create_intcode(memory, num_ints);
        }
    }

    return prog;
}

intcode_t* create_intcode(int64_t* const memory, const size_t memory_size)
{
    intcode_t* prog = NULL;
    if (memory != NULL)
    {
        prog = (intcode_t*) malloc(sizeof(intcode_t));
        if (prog != NULL)
        {
            prog->memory        = memory;
            prog->memory_size   = memory_size;
            prog->head          = 0;
            prog->relative_base = 0;
            prog->io_mode       = INT_CODE_STD_IO;
            prog->std_io_in     = stdin;
            prog->std_io_out    = stdout;
            prog->mem_io_in     = NULL;
            prog->mem_io_out    = NULL;
        }
    }
    return prog;
}

void destroy_intcode(intcode_t* const prog)
{
    if (prog != NULL)
    {
        if (prog->memory != NULL)
        {
            free(prog->memory);
        }
        free(prog);
    }
}

int set_mem_value(intcode_t* const prog, const size_t address, const int64_t value)
{
    int success = 0;
    if ((prog != NULL) && (prog->memory != NULL))
    {
        if (address >= prog->memory_size)
        {
            /*Allocate more memory*/
            size_t new_size = address + 1;
            prog->memory    = realloc(prog->memory, sizeof(int64_t) * new_size);
            if (prog->memory != NULL)
            {
                memset(prog->memory + prog->memory_size,
                       0,
                       sizeof(int64_t) * (new_size - prog->memory_size));
                prog->memory_size = new_size;
            }
        }

        if (prog->memory != NULL)
        {
            prog->memory[address] = value;
            success               = 1;
        }
    }
    return success;
}

int64_t get_mem_value(const intcode_t* const prog, const size_t address)
{
    int64_t value = 0;
    if (prog != NULL)
    {
        if (address >= prog->memory_size)
        {
            /*Address is outside of memory, i.e. not set.*/
            /*Even if memory was resized to fit, 0 would be returned.*/
            /*So let's wait with allocating until values are stored there.*/
            value = 0;
        }

        if (prog->memory != NULL)
        {
            value = prog->memory[address];
        }
    }
    return value;
}

void print_intcode(const intcode_t* const prog)
{
    if (prog != NULL)
    {
        /*Print program*/
        int op_code       = get_opcode(prog->memory[0]);
        size_t inst_index = 0;
        size_t inst_size  = get_instruction_size(op_code);
        for (size_t i = 0; i < prog->memory_size; i++)
        {
            printf("%ld", prog->memory[i]);
            if (((inst_index + 1) % inst_size) == 0)
            {
                printf("\n");
                if ((i + 1) < prog->memory_size)
                {
                    inst_index = 0;
                    op_code    = get_opcode(prog->memory[i + 1]);
                    inst_size  = get_instruction_size(op_code);
                }
            }
            else if ((i + 1) < prog->memory_size)
            {
                inst_index++;
                printf(" ");
            }
        }
        printf("\n");
    }
}

void set_io_mode(intcode_t* const prog, const intcode_io_mode_t mode)
{
    if (prog != NULL)
    {
        prog->io_mode = mode;
    }
}

void set_mem_io_in(intcode_t* const prog, intcode_io_mem_t* const input_store)
{
    if (prog != NULL)
    {
        prog->mem_io_in = input_store;
    }
}

void set_mem_io_out(intcode_t* const prog, intcode_io_mem_t* const output_store)
{
    if (prog != NULL)
    {
        prog->mem_io_out = output_store;
    }
}

void set_std_io_in(intcode_t* const prog, FILE* const input_stream)
{
    if (prog != NULL)
    {
        prog->std_io_in = input_stream;
    }
}
void set_std_io_out(intcode_t* const prog, FILE* const output_stream)
{
    if (prog != NULL)
    {
        prog->std_io_out = output_stream;
    }
}

intcode_t* copy_intcode(const intcode_t* const prog)
{
    intcode_t* copy = NULL;
    if (prog != NULL)
    {
        if ((prog->memory != NULL) && (prog->memory_size > 0))
        {
            int64_t* memory_copy = (int64_t*) malloc(sizeof(int64_t) * prog->memory_size);
            if (memory_copy != NULL)
            {
                for (size_t i = 0; i < prog->memory_size; ++i)
                {
                    memory_copy[i] = prog->memory[i];
                }
            }
            copy = create_intcode(memory_copy, prog->memory_size);
        }
    }
    return copy;
}

int output_intcode(const intcode_t* const prog)
{
    int out = -1;
    if ((prog != NULL) && (prog->memory != NULL) && (prog->memory_size > 0))
    {
        out = prog->memory[0];
    }
    return out;
}

int execute(intcode_t* const prog)
{
    int ret = INT_CODE_ERROR;
    if (prog != NULL)
    {
        ret = INT_CODE_CONTINUE;
        while (ret == INT_CODE_CONTINUE)
        {
            int op_code;
            ret = execute_head_block(prog, &op_code);
        }
    }
    return ret;
}

int execute_head_block(intcode_t* const prog, int* const op_code)
{
    int ret = INT_CODE_ERROR;
    if (prog != NULL)
    {
        size_t head = prog->head;
        *op_code    = get_opcode(get_mem_value(prog, prog->head));
#ifdef DEBUG
        printf("Op Code: %d\n", *op_code);
#endif
        size_t inst_size = get_instruction_size(*op_code);

#ifdef DEBUG
        for (int i = 0; i < inst_size; ++i)
        {
            printf("%ld ", prog->memory[head + i]);
        }
        printf("\n");
#endif
        if (*op_code == OP_CODE_HALT)
        {
            ret = INT_CODE_HALT;
        }
        else if (is_valid_opcode(*op_code))
        {
            int64_t parameters[inst_size - 1];
            int parameter_modes[inst_size - 1];
            int store_param = inst_size - 2;
            if ((*op_code == OP_CODE_OUTPUT) || (*op_code == OP_CODE_JMP_IF_TRUE) ||
                (*op_code == OP_CODE_JMP_IF_FALSE) || (*op_code == OP_CODE_ADJUST_REL_BASE))
            {
                store_param = INTCODE_NO_STORE;
            }
            get_parameter_modes(get_mem_value(prog, head), inst_size - 1, parameter_modes);
            if (get_parameter_values(prog, inst_size - 1, store_param, parameter_modes, parameters))
            {
#ifdef DEBUG
                for (int i = 0; i < inst_size - 1; i++)
                {
                    printf("%d\t%ld\n", parameter_modes[i], parameters[i]);
                }
#endif
                ret = get_op_func(*op_code)(prog, parameters);
            }
        }
    }
    return ret;
}

int add_op(intcode_t* const prog, const int64_t* const parameters)
{
    int op_ret = INT_CODE_ERROR;
    /*TODO add boundary checks*/
    /*Assuming parameters has the correct size*/
    if ((prog != NULL) && (parameters != NULL))
    {
        int64_t first  = parameters[0];
        int64_t second = parameters[1];
        int64_t result = parameters[2];
        int ret        = set_mem_value(prog, result, (first + second));
        if (ret != 0)
        {
            prog->head += get_instruction_size(OP_CODE_ADD);
            op_ret = INT_CODE_CONTINUE;
        }
    }
    return op_ret;
}

int multiply_op(intcode_t* const prog, const int64_t* const parameters)
{
    int op_ret = INT_CODE_ERROR;
    /*TODO add boundary checks*/
    /*Assuming parameters has the correct size*/
    if ((prog != NULL) && (parameters != NULL))
    {
        int64_t first  = parameters[0];
        int64_t second = parameters[1];
        int64_t result = parameters[2];
        int ret        = set_mem_value(prog, result, (first * second));
        if (ret != 0)
        {
            prog->head += get_instruction_size(OP_CODE_MULT);
            op_ret = INT_CODE_CONTINUE;
        }
    }
    return op_ret;
}

int input_op(intcode_t* const prog, const int64_t* const parameters)
{
    int op_ret = INT_CODE_ERROR;
    if ((prog != NULL) && (parameters != NULL))
    {
        int64_t val;
        if (prog->io_mode == INT_CODE_STD_IO)
        {
            if (read_from_io_std(prog->std_io_in, &val))
            {
                int ret = set_mem_value(prog, parameters[0], val);
                if (ret != 0)
                {
                    prog->head += get_instruction_size(OP_CODE_INPUT);
                    op_ret = INT_CODE_CONTINUE;
                }
            }
        }
        else if (prog->io_mode == INT_CODE_MEM_IO)
        {
            pthread_mutex_lock(&prog->mem_io_in->mut);
            while (prog->mem_io_in->consumed)
            {
                pthread_cond_wait(&prog->mem_io_in->cond, &prog->mem_io_in->mut);
            }
            read_from_io_mem(prog->mem_io_in, &val);
            pthread_cond_signal(&prog->mem_io_in->cond);
            pthread_mutex_unlock(&prog->mem_io_in->mut);

            int ret = set_mem_value(prog, parameters[0], val);
            if (ret != 0)
            {
                prog->head += get_instruction_size(OP_CODE_INPUT);
                op_ret = INT_CODE_CONTINUE;
            }
        }
    }
    return op_ret;
}

int output_op(intcode_t* const prog, const int64_t* const parameters)
{
    /*TODO add boundary checks*/
    int op_ret = INT_CODE_ERROR;
    if (parameters != NULL)
    {
        if (prog->io_mode == INT_CODE_STD_IO)
        {
            write_to_io_std(prog->std_io_out, parameters[0]);
        }
        else if (prog->io_mode == INT_CODE_MEM_IO)
        {
            pthread_mutex_lock(&prog->mem_io_out->mut);
            while (!prog->mem_io_out->consumed)
            {
                /*spin*/
                pthread_cond_signal(&prog->mem_io_out->cond);
                pthread_cond_wait(&prog->mem_io_out->cond, &prog->mem_io_out->mut);
            }
            write_to_io_mem(prog->mem_io_out, parameters[0]);
            pthread_cond_signal(&prog->mem_io_out->cond);
            pthread_mutex_unlock(&prog->mem_io_out->mut);
        }
        prog->head += get_instruction_size(OP_CODE_OUTPUT);
        op_ret = INT_CODE_CONTINUE;
    }
    return op_ret;
}

int jmp_if_true_op(intcode_t* const prog, const int64_t* const parameters)
{
    /*TODO add boundary checks*/
    int op_ret = INT_CODE_ERROR;
    if (parameters != NULL)
    {
        if (parameters[0] != 0)
        {
            prog->head = parameters[1];
        }
        else
        {
            prog->head += get_instruction_size(OP_CODE_JMP_IF_TRUE);
        }
        op_ret = INT_CODE_CONTINUE;
    }
    return op_ret;
}

int jmp_if_false_op(intcode_t* const prog, const int64_t* const parameters)
{
    /*TODO add boundary checks*/
    int op_ret = INT_CODE_ERROR;
    if (parameters != NULL)
    {
        if (parameters[0] == 0)
        {
            prog->head = parameters[1];
        }
        else
        {
            prog->head += get_instruction_size(OP_CODE_JMP_IF_FALSE);
        }
        op_ret = INT_CODE_CONTINUE;
    }
    return op_ret;
}

int is_less_op(intcode_t* const prog, const int64_t* const parameters)
{
    /*TODO add boundary checks*/
    int op_ret = INT_CODE_ERROR;
    if (parameters != NULL)
    {
        int ret = 0;
        if (parameters[0] < parameters[1])
        {
            ret = set_mem_value(prog, parameters[2], 1);
        }
        else
        {
            ret = set_mem_value(prog, parameters[2], 0);
        }
        if (ret != 0)
        {
            prog->head += get_instruction_size(OP_CODE_IS_LESS);
            op_ret = INT_CODE_CONTINUE;
        }
    }
    return op_ret;
}
int is_equals_op(intcode_t* const prog, const int64_t* const parameters)
{
    /*TODO add boundary checks*/
    int op_ret = INT_CODE_ERROR;
    if (parameters != NULL)
    {
        int ret = 0;
        if (parameters[0] == parameters[1])
        {
            ret = set_mem_value(prog, parameters[2], 1);
        }
        else
        {
            ret = set_mem_value(prog, parameters[2], 0);
        }
        if (ret != 0)
        {
            prog->head += get_instruction_size(OP_CODE_IS_EQUALS);
            op_ret = INT_CODE_CONTINUE;
        }
    }
    return op_ret;
}

int adjust_rel_base_op(intcode_t* const prog, const int64_t* const parameters)
{
    int op_ret = INT_CODE_ERROR;
    if ((prog != NULL) && (parameters != NULL))
    {
        prog->relative_base += parameters[0];
        prog->head += get_instruction_size(OP_CODE_ADJUST_REL_BASE);
        op_ret = INT_CODE_CONTINUE;
    }
    return op_ret;
}

int error_op(intcode_t* const prog, const int64_t* const parameters)
{
    return INT_CODE_ERROR;
}

static void get_size_info(const char* const file_path,
                          size_t* const total_chars,
                          size_t* const amount_integers)
{
    /*Again, inefficient but lazy*/
    if (file_path != NULL)
    {
        FILE* fp = fopen(file_path, "r");
        if (fp != NULL)
        {
            char ch = fgetc(fp);
            while (ch != EOF)
            {
                *total_chars += 1;
                if (ch == ',')
                {
                    *amount_integers += 1;
                }
                ch = fgetc(fp);
            }
            *amount_integers += 1;
            fclose(fp);
        }
    }
}

static size_t get_instruction_size(const int op_code)
{
    size_t inst_size = 1;
    if ((op_code == OP_CODE_ADD) || (op_code == OP_CODE_MULT) || (op_code == OP_CODE_IS_LESS) ||
        (op_code == OP_CODE_IS_EQUALS))
    {
        inst_size = 4;
    }
    else if ((op_code == OP_CODE_JMP_IF_TRUE) || (op_code == OP_CODE_JMP_IF_FALSE))
    {
        inst_size = 3;
    }
    else if ((op_code == OP_CODE_INPUT) || (op_code == OP_CODE_OUTPUT) ||
             (op_code == OP_CODE_ADJUST_REL_BASE))
    {
        inst_size = 2;
    }
    else if (op_code == OP_CODE_HALT)
    {
        inst_size = 1;
    }
    return inst_size;
}

static int get_opcode(const int64_t number)
{
    int op_code = 0;
    if (number > 99)
    {
        op_code = number % 100;
    }
    else
    {
        op_code = number;
    }
    return op_code;
}

static int is_valid_opcode(const int op_code)
{
    return (op_code >= 1 && op_code <= 9) || (op_code == OP_CODE_HALT);
}

static void get_parameter_modes(const int64_t number,
                                const size_t num_parameters,
                                int* const parameter_modes)
{
    if (NULL != parameter_modes)
    {
        int64_t modes = number / 100;
        for (size_t i = 0; i < num_parameters; i++)
        {
            int mode           = modes % 10;
            parameter_modes[i] = mode;
            modes /= 10;
        }
    }
}

static int get_parameter_values(const intcode_t* const prog,
                                const size_t num_parameters,
                                const int store_param,
                                const int* const parameter_modes,
                                int64_t* const parameters)
{
    int no_error = 0;
    /*Assuming range checks are made by caller*/
    if ((prog != NULL) && (parameter_modes != NULL) && (parameters != NULL))
    {
        no_error = 1;
        for (size_t i = 0; i < num_parameters; i++)
        {
            int64_t memory_val = get_mem_value(prog, prog->head + i + 1);
            int64_t param_val  = 0;
            if (parameter_modes[i] == PARAM_MODE_POSITION)
            {
                /*TODO check boundaries*/
                if ((store_param != INTCODE_NO_STORE) && (store_param == i))
                {
                    /*The parameter value is the address where the result of op is stored.*/
                    param_val = memory_val;
                }
                else
                {
                    /*The parameter value is stored at the address*/
                    param_val = get_mem_value(prog, memory_val);
                }
            }
            else if (parameter_modes[i] == PARAM_MODE_IMMEDIATE)
            {
                param_val = memory_val;
            }
            else if (parameter_modes[i] == PARAM_MODE_RELATIVE)
            {
                /*TODO check boundaries*/
                if ((store_param != INTCODE_NO_STORE) && (store_param == i))
                {
                    /*The parameter value is the address where the result of op is stored.*/
                    param_val = memory_val + prog->relative_base;
                }
                else
                {
                    /*The parameter value is stored at the address*/
                    param_val = get_mem_value(prog, memory_val + prog->relative_base);
                }
            }
            else
            {
                no_error = 0;
                break;
            }
            parameters[i] = param_val;
        }
    }
    return no_error;
}

static intcode_op_f get_op_func(const int op_code)
{
    switch (op_code)
    {
        case OP_CODE_ADD:
            return add_op;
        case OP_CODE_MULT:
            return multiply_op;
        case OP_CODE_INPUT:
            return input_op;
        case OP_CODE_OUTPUT:
            return output_op;
        case OP_CODE_IS_LESS:
            return is_less_op;
        case OP_CODE_IS_EQUALS:
            return is_equals_op;
        case OP_CODE_JMP_IF_TRUE:
            return jmp_if_true_op;
        case OP_CODE_JMP_IF_FALSE:
            return jmp_if_false_op;
        case OP_CODE_ADJUST_REL_BASE:
            return adjust_rel_base_op;
        default:
            return error_op;
    }
}

static void write_to_io_std(FILE* const stream, const int64_t value)
{
    if (stream != NULL)
    {
        fprintf(stream, "%ld\n", value);
    }
}

static int read_from_io_std(FILE* const stream, int64_t* value)
{
    int success = 0;
    if (stream != NULL)
    {
        if (fscanf(stream, "%ld", value) == 1)
        {
            success = 1;
        }
    }
    return success;
}

static void write_to_io_mem(intcode_io_mem_t* const storage, const int64_t value)
{
    if (storage != NULL)
    {
        storage->value    = value;
        storage->consumed = 0;
    }
}

static void read_from_io_mem(intcode_io_mem_t* const storage, int64_t* value)
{
    if ((storage != NULL) && (value != NULL))
    {
        *value            = storage->value;
        storage->consumed = 1;
    }
}
