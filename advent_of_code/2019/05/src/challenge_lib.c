#include "challenge/challenge_lib.h"

#include "stdio.h"
#include "string.h"

#define INTCODE_DELIM ","

static void get_size_info(const char* const file_path,
                          size_t* const total_chars,
                          size_t* const amount_integers);

static size_t get_instruction_size(const int op_code);
static void move_head(intcode_t* const prog);

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
        int op_code = prog->memory[0];
        size_t inst_size = get_instruction_size(op_code);
        for (size_t i = 0; i < prog->memory_size; i++)
        {
            printf("%d", prog->memory[i]);
            if (((i + 1) % inst_size) == 0)
            {
                printf("\n");
                if ((i + 1) < prog->memory_size)
                {
                    inst_size = get_instruction_size(prog->memory[i + 1]);
                }
            }
            else if ((i + 1) < prog->memory_size)
            {
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
            ret = execute_head_block(prog);
            if (ret == INT_CODE_CONTINUE)
            {
                move_head(prog);
            }
        }
    }
    return ret;
}

int execute_head_block(intcode_t* const prog)
{
    int ret = INT_CODE_ERROR;
    if ((prog != NULL) && (prog->memory != NULL))
    {
        size_t head = prog->head;
        if (head < prog->memory_size)
        {
            int op_code = prog->memory[head];
            int first, second, result;
            size_t inst_size = get_instruction_size(op_code);
            switch (op_code)
            {
                case OP_CODE_ADD:
                    if (head < (prog->memory_size - inst_size + 1))
                    {
                        first  = prog->memory[head + 1];
                        second = prog->memory[head + 2];
                        result = prog->memory[head + 3];
                        add_op(prog, first, second, result);
                        ret = INT_CODE_CONTINUE;
                    }
                    else
                    {
                        ret = INT_CODE_ERROR;
                    }
                    break;
                case OP_CODE_MULT:
                    if (head < (prog->memory_size - inst_size + 1))
                    {
                        first  = prog->memory[head + 1];
                        second = prog->memory[head + 2];
                        result = prog->memory[head + 3];
                        multiply_op(prog, first, second, result);
                        ret = INT_CODE_CONTINUE;
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

void add_op(intcode_t* const prog,
            const int first,
            const int second,
            const int result)
{
    if ((prog != NULL) && (prog->memory != NULL))
    {
        if ((result < prog->memory_size) && (first < prog->memory_size) &&
            (second < prog->memory_size))
        {
            prog->memory[result] = prog->memory[first] + prog->memory[second];
        }
    }
}

void multiply_op(intcode_t* const prog,
                 const int first,
                 const int second,
                 const int result)
{
    if ((prog != NULL) && (prog->memory != NULL))
    {
        if ((result < prog->memory_size) && (first < prog->memory_size) &&
            (second < prog->memory_size))
        {
            prog->memory[result] = prog->memory[first] * prog->memory[second];
        }
    }
}

static void move_head(intcode_t* const prog)
{
    if (prog != NULL)
    {
        prog->head += get_instruction_size(prog->memory[prog->head]);
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
    size_t inst_size = 0;
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
