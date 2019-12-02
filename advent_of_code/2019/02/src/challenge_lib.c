#include "challenge/challenge_lib.h"

#include "stdio.h"
#include "string.h"

#define INTCODE_DELIM ","

static void get_size_info(const char* const file_path,
                          size_t* const total_chars,
                          size_t* const amount_integers);

intcode_t* read_intcode(const char* const file_path)
{
    intcode_t* prog = NULL;
    if (file_path != NULL)
    {
        size_t num_chars = 0;
        size_t num_ints  = 0;
        get_size_info(file_path, &num_chars, &num_ints);

        char* str    = (char*)malloc(sizeof(char) * num_chars);
        int* content = (int*)malloc(sizeof(int) * num_ints);
        FILE* fp     = fopen(file_path, "r");
        if ((fp != NULL) && (str != NULL) && (content != NULL))
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
                content[index++] = atoi(token);
                token            = strtok(NULL, INTCODE_DELIM);
            }
            free(str);

            prog = create_intcode(content, num_ints);
        }
    }

    return prog;
}

intcode_t* create_intcode(int* const content, const size_t content_size)
{
    intcode_t* prog = NULL;
    if (content != NULL)
    {
        prog = (intcode_t*)malloc(sizeof(intcode_t));
        if (prog != NULL)
        {
            prog->content      = content;
            prog->content_size = content_size;
            prog->head         = 0;
        }
    }
    return prog;
}

void destroy_intcode(intcode_t* const prog)
{
    if (prog != NULL)
    {
        if (prog->content != NULL)
        {
            free(prog->content);
        }
        free(prog);
    }
}


int execute(intcode_t* const prog)
{
    /*TODO: remove magic numbers*/
    int ret = 0;
    if (prog != NULL)
    {
        ret = 2;
        while (ret == 2)
        {
            ret = execute_head_block(prog);
            if (ret == 2)
            {
                /*TODO use return value here*/
                move_head(prog);
            }
        }
    }
    return ret;
}

int execute_head_block(intcode_t* const prog)
{
    int ret = 0;
    if ((prog != NULL) && (prog->content != NULL))
    {
        size_t head = prog->head;
        int op_code = prog->content[head];
        int first, second, result;

        /*TODO: remove magic numbers*/
        switch (op_code)
        {
            case 1:
                /*TODO: add boundary checks*/
                first  = prog->content[head + 1];
                second = prog->content[head + 2];
                result = prog->content[head + 3];
                add_op(prog, first, second, result);
                ret = 2;
                break;
            case 2:
                /*TODO: add boundary checks*/
                first  = prog->content[head + 1];
                second = prog->content[head + 2];
                result = prog->content[head + 3];
                multiply_op(prog, first, second, result);
                ret = 2;
                break;
            case 99:
                ret = 1;
                break;
            default:
                ret = -1;
                break;
        }
    }
    return ret;
}

void add_op(intcode_t* const prog, const int first, const int second, const int result)
{
    if ((prog != NULL) && (prog->content != NULL))
    {
        /*TODO check boundaries*/
        prog->content[result] = prog->content[first] + prog->content[second];
    }
}

void multiply_op(intcode_t* const prog, const int first, const int second, const int result)
{
    if ((prog != NULL) && (prog->content != NULL))
    {
        /*TODO check boundaries*/
        prog->content[result] = prog->content[first] * prog->content[second];
    }
}

int move_head(intcode_t* const prog)
{
    int ret = 0;
    if (prog != NULL)
    {
        /*TODO: remove magic number*/
        prog->head += 4;
        if (prog->head < prog->content_size)
        {
            ret = 1;
        }
    }
    return ret;
}


static void
get_size_info(const char* const file_path, size_t* const total_chars, size_t* const amount_integers)
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
