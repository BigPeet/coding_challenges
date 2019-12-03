#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_02 FILE_PATH.\n");
        return 0;
    }

    intcode_t* prog = read_intcode(argv[1]);
    if (prog != NULL)
    {
        /*Apply fix for Part 1 of the Task*/
        if ((prog->memory != NULL) && (prog->memory_size > 3))
        {
            prog->memory[1] = 12;
            prog->memory[2] = 2;
        }

        int ret = execute(prog);
        switch (ret)
        {
            case 0:
            case -1:
                printf("Something went wrong!\n");
                break;
            case 1:
                printf("We reached 99.\n");
                print_intcode(prog);
                break;
            case 2:
                printf("We ran out of stuff??\n");
                break;
            default:
                printf("Default: Something went wrong!\n");
                break;
        }
        destroy_intcode(prog);
    }
    return 0;
}
