/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-05
 *
 */

#include "challenge/challenge_lib.h"

#include "stdio.h"
#include "string.h"

#define INTCODE_DELIM ","
#define INTCODE_NO_STORE -1

typedef int (*intcode_op_f)(intcode_t* const, const int* const);

static void get_size_info(const char* const file_path,
                          size_t* const total_chars,
                          size_t* const amount_integers);

static size_t get_instruction_size(const int op_code);
static int get_opcode(const int number);
static int is_valid_opcode(const int op_code);

static intcode_op_f get_op_func(const int op_code);
static void get_parameter_modes(const int number,
                                const size_t num_parameters,
                                const int store_param,
                                int* const parameter_modes);

static int get_parameter_values(const int* const memory,
                                const size_t head,
                                const size_t num_parameters,
                                const int* const parameter_modes,
                                int* const parameters);


intcode_t* read_intcode(const char* const file_path)
{
    intcode_t* prog = NULL;
    if (file_path != NULL)
    {
        size_t num_chars = 0;
        size_t num_ints  = 0;
        get_size_info(file_path, &num_chars, &num_ints);

        char* str   = (char*) malloc(sizeof(char) * num_chars);
        int* memory = (int*) malloc(sizeof(int) * num_ints);
        FILE* fp    = fopen(file_path, "r");
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
                memory[index++] = atoi(token);
                token           = strtok(NULL, INTCODE_DELIM);
            }
            free(str);

            prog = create_intcode(memory, num_ints);
        }
    }

    return prog;
}

intcode_t* create_intcode(int* const memory, const size_t memory_size)
{
    intcode_t* prog = NULL;
    if (memory != NULL)
    {
        prog = (intcode_t*) malloc(sizeof(intcode_t));
        if (prog != NULL)
        {
            prog->memory      = memory;
            prog->memory_size = memory_size;
            prog->head        = 0;
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
            printf("%d", prog->memory[i]);
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

intcode_t* copy_intcode(const intcode_t* const prog)
{
    intcode_t* copy = NULL;
    if (prog != NULL)
    {
        if ((prog->memory != NULL) && (prog->memory_size > 0))
        {
            int* memory_copy = (int*) malloc(sizeof(int) * prog->memory_size);
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
    if ((prog != NULL) && (prog->memory != NULL))
    {
        size_t head = prog->head;
        if (head < prog->memory_size)
        {
            *op_code         = get_opcode(prog->memory[head]);
            size_t inst_size = get_instruction_size(*op_code);
            if (*op_code == OP_CODE_HALT)
            {
                ret = INT_CODE_HALT;
            }
            else if (is_valid_opcode(*op_code))
            {
                if (head < (prog->memory_size - inst_size + 1))
                {
                    int parameters[inst_size - 1];
                    int parameter_modes[inst_size - 1];
                    int store_param = inst_size - 2;
                    if ((*op_code == OP_CODE_OUTPUT) || (*op_code == OP_CODE_JMP_IF_TRUE) ||
                        (*op_code == OP_CODE_JMP_IF_FALSE))
                    {
                        store_param = INTCODE_NO_STORE;
                    }
                    get_parameter_modes(prog->memory[head],
                                        inst_size - 1,
                                        store_param,
                                        parameter_modes);
                    if (get_parameter_values(prog->memory,
                                             head,
                                             inst_size - 1,
                                             parameter_modes,
                                             parameters))
                    {
                        ret = get_op_func(*op_code)(prog, parameters);
                    }
                }
            }
        }
    }
    return ret;
}

int add_op(intcode_t* const prog, const int* const parameters)
{
    int ret = INT_CODE_ERROR;
    /*TODO add boundary checks*/
    /*Assuming parameters has the correct size*/
    if ((prog != NULL) && (prog->memory != NULL) && (parameters != NULL))
    {
        int first  = parameters[0];
        int second = parameters[1];
        int result = parameters[2];
        if (result < prog->memory_size)
        {
            prog->memory[result] = first + second;
            prog->head += get_instruction_size(OP_CODE_ADD);
            ret = INT_CODE_CONTINUE;
        }
    }
    return ret;
}

int multiply_op(intcode_t* const prog, const int* const parameters)
{
    int ret = INT_CODE_ERROR;
    /*TODO add boundary checks*/
    /*Assuming parameters has the correct size*/
    if ((prog != NULL) && (prog->memory != NULL) && (parameters != NULL))
    {
        int first  = parameters[0];
        int second = parameters[1];
        int result = parameters[2];
        if (result < prog->memory_size)
        {
            prog->memory[result] = first * second;
            prog->head += get_instruction_size(OP_CODE_MULT);
            ret = INT_CODE_CONTINUE;
        }
    }
    return ret;
}

int input_op(intcode_t* const prog, const int* const parameters)
{
    int ret = INT_CODE_ERROR;
    if ((prog != NULL) && (parameters != NULL))
    {
        int val;
        printf("Input: ");
        if (scanf("%d", &val) == 1)
        {
            /*TODO add boundary check*/
            prog->memory[parameters[0]] = val;
            prog->head += get_instruction_size(OP_CODE_INPUT);
            ret = INT_CODE_CONTINUE;
        }
    }
    return ret;
}

int output_op(intcode_t* const prog, const int* const parameters)
{
    /*TODO add boundary checks*/
    int ret = INT_CODE_ERROR;
    if (parameters != NULL)
    {
        printf("Output: %d\n", parameters[0]);
        prog->head += get_instruction_size(OP_CODE_OUTPUT);
        ret = INT_CODE_CONTINUE;
    }
    return ret;
}

int jmp_if_true_op(intcode_t* const prog, const int* const parameters)
{
    /*TODO add boundary checks*/
    int ret = INT_CODE_ERROR;
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
        ret = INT_CODE_CONTINUE;
    }
    return ret;
}

int jmp_if_false_op(intcode_t* const prog, const int* const parameters)
{
    /*TODO add boundary checks*/
    int ret = INT_CODE_ERROR;
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
        ret = INT_CODE_CONTINUE;
    }
    return ret;
}

int is_less_op(intcode_t* const prog, const int* const parameters)
{
    /*TODO add boundary checks*/
    int ret = INT_CODE_ERROR;
    if (parameters != NULL)
    {
        if (parameters[0] < parameters[1])
        {
            prog->memory[parameters[2]] = 1;
        }
        else
        {
            prog->memory[parameters[2]] = 0;
        }
        prog->head += get_instruction_size(OP_CODE_IS_LESS);
        ret = INT_CODE_CONTINUE;
    }
    return ret;
}
int is_equals_op(intcode_t* const prog, const int* const parameters)
{
    /*TODO add boundary checks*/
    int ret = INT_CODE_ERROR;
    if (parameters != NULL)
    {
        if (parameters[0] == parameters[1])
        {
            prog->memory[parameters[2]] = 1;
        }
        else
        {
            prog->memory[parameters[2]] = 0;
        }
        prog->head += get_instruction_size(OP_CODE_IS_EQUALS);
        ret = INT_CODE_CONTINUE;
    }
    return ret;
}

int error_op(intcode_t* const prog, const int* const parameters)
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
    if ((op_code == OP_CODE_ADD) || (op_code == OP_CODE_MULT) ||
        (op_code == OP_CODE_IS_LESS) ||
        (op_code == OP_CODE_IS_EQUALS))
    {
        inst_size = 4;
    }
    else if ((op_code == OP_CODE_JMP_IF_TRUE) ||
             (op_code == OP_CODE_JMP_IF_FALSE))
    {
        inst_size = 3;
    }
    else if ((op_code == OP_CODE_INPUT) || (op_code == OP_CODE_OUTPUT))
    {
        inst_size = 2;
    }
    else if (op_code == OP_CODE_HALT)
    {
        inst_size = 1;
    }
    return inst_size;
}

static int get_opcode(const int number)
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
    return (op_code >= 1 && op_code <= 8) || (op_code == OP_CODE_HALT);
}

static void get_parameter_modes(const int number,
                                const size_t num_parameters,
                                const int store_param,
                                int* const parameter_modes)
{
    if (NULL != parameter_modes)
    {
        int modes = number / 100;
        for (size_t i = 0; i < num_parameters; i++)
        {
            int mode           = modes % 10;
            parameter_modes[i] = mode;
            modes /= 10;
        }
        if ((store_param != INTCODE_NO_STORE) &&
            (store_param < num_parameters))
        {
            /*Storage parameters always use the address, i.e. immediate mode*/
            parameter_modes[store_param] = PARAM_MODE_IMMEDIATE;
        }
    }
}

static int get_parameter_values(const int* const memory,
                                const size_t head,
                                const size_t num_parameters,
                                const int* const parameter_modes,
                                int* const parameters)
{
    int no_error = 0;
    /*Assuming range checks are made by caller*/
    if ((memory != NULL) && (parameter_modes != NULL) && (parameters != NULL))
    {
        no_error = 1;
        for (size_t i = 0; i < num_parameters; i++)
        {
            int memory_val = memory[head + i + 1];
            int param_val  = 0;
            if (parameter_modes[i] == PARAM_MODE_POSITION)
            {
                /*TODO check boundaries*/
                param_val = memory[memory_val];
            }
            else if (parameter_modes[i] == PARAM_MODE_IMMEDIATE)
            {
                param_val = memory_val;
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
        default:
            return error_op;
    }
}
