#include "challenge/challenge_lib.h"

#include "stdio.h"
#include "string.h"

#define INTCODE_DELIM ","
#define INTCODE_NO_STORE -1

static void get_size_info(const char* const file_path,
                          size_t* const total_chars,
                          size_t* const amount_integers);

static size_t get_instruction_size(const int op_code);
static void move_head(intcode_t* const prog, const int op_code);
static int get_opcode(const int number);
static int is_valid_opcode(const int op_code);

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
            if (ret == INT_CODE_CONTINUE)
            {
                move_head(prog, op_code);
            }
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
            switch (*op_code)
            {
                case OP_CODE_ADD:
                    if (head < (prog->memory_size - inst_size + 1))
                    {
                        int parameters[inst_size - 1];
                        int parameter_modes[inst_size - 1];
                        get_parameter_modes(prog->memory[head],
                                            inst_size - 1,
                                            inst_size - 2,
                                            parameter_modes);
                        if (get_parameter_values(prog->memory,
                                                 head,
                                                 inst_size - 1,
                                                 parameter_modes,
                                                 parameters))
                        {
                            add_op(prog, parameters);
                            ret = INT_CODE_CONTINUE;
                        }
                        else
                        {
                            ret = INT_CODE_ERROR;
                        }
                    }
                    else
                    {
                        ret = INT_CODE_ERROR;
                    }
                    break;
                case OP_CODE_MULT:
                    if (head < (prog->memory_size - inst_size + 1))
                    {
                        int parameters[inst_size - 1];
                        int parameter_modes[inst_size - 1];
                        get_parameter_modes(prog->memory[head],
                                            inst_size - 1,
                                            inst_size - 2,
                                            parameter_modes);
                        if (get_parameter_values(prog->memory,
                                                 head,
                                                 inst_size - 1,
                                                 parameter_modes,
                                                 parameters))
                        {
                            multiply_op(prog, parameters);
                            ret = INT_CODE_CONTINUE;
                        }
                        else
                        {
                            ret = INT_CODE_ERROR;
                        }
                    }
                    else
                    {
                        ret = INT_CODE_ERROR;
                    }
                    break;
                case OP_CODE_HALT:
                    ret = INT_CODE_HALT;
                    break;
                default:
                    ret = INT_CODE_ERROR;
                    break;
            }
        }
    }
    return ret;
}

void add_op(intcode_t* const prog, const int* const parameters)
{
    /*Assuming parameters has the correct size*/
    if ((prog != NULL) && (prog->memory != NULL) && (parameters != NULL))
    {
        int first  = parameters[0];
        int second = parameters[1];
        int result = parameters[2];
        if (result < prog->memory_size)
        {
            prog->memory[result] = first + second;
        }
    }
}

void multiply_op(intcode_t* const prog, const int* const parameters)
{
    /*Assuming parameters has the correct size*/
    if ((prog != NULL) && (prog->memory != NULL) && (parameters != NULL))
    {
        int first  = parameters[0];
        int second = parameters[1];
        int result = parameters[2];
        if (result < prog->memory_size)
        {
            prog->memory[result] = first * second;
        }
    }
}

static void move_head(intcode_t* const prog, const int op_code)
{
    if (prog != NULL)
    {
        prog->head += get_instruction_size(op_code);
    }
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
    if ((op_code == OP_CODE_ADD) || (op_code == OP_CODE_MULT))
    {
        inst_size = 4;
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
    return ((op_code == OP_CODE_ADD) || (op_code == OP_CODE_MULT) ||
            (op_code == OP_CODE_HALT));
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
            /*TODO: if it is STORE parameter, then it needs to be the immediate
             * mode
             * Since the value there is the address to store*/
        }
        if ((store_param != INTCODE_NO_STORE) && (store_param < num_parameters))
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
