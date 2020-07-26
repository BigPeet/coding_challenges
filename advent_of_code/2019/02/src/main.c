/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-02
 *
 */

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
        /*Create copy for PART 1 of the Task*/
        intcode_t* copy = copy_intcode(prog);
        if (copy != NULL)
        {
            /*Apply fix for Part 1 of the Task*/
            if ((copy->memory != NULL) && (copy->memory_size > 3))
            {
                copy->memory[1] = 12;
                copy->memory[2] = 2;
            }

            int ret = execute(copy);
            switch (ret)
            {
                case 0:
                case -1:
                    printf("Something went wrong!\n");
                    break;
                case 1:
                    printf("We reached 99.\n");
                    print_intcode(copy);
                    printf("Output of program: %d\n", output_intcode(copy));
                    break;
                case 2:
                    printf("We ran out of stuff??\n");
                    break;
                default:
                    printf("Default: Something went wrong!\n");
                    break;
            }
            destroy_intcode(copy);
        }

        /*PART 2 of the Task*/
        int noun   = 0;
        int verb   = 0;
        int output = -1;
        int target = 19690720;
        while (output != target)
        {
            intcode_t* copy = copy_intcode(prog);
            if (copy != NULL)
            {
                /*Apply noun and verb*/
                if ((copy->memory != NULL) && (copy->memory_size > 3))
                {
                    copy->memory[1] = noun;
                    copy->memory[2] = verb;
                    int ret         = execute(copy);
                    switch (ret)
                    {
                        case INT_CODE_HALT:
                            output = output_intcode(copy);
                            break;
                        default:
                            output = -1;
                            break;
                    }
                }
                destroy_intcode(copy);

                if (output == target)
                {
                    printf("Found solution: Noun = %d, Verb = %d.\n", noun, verb);
                }
                else
                {
                    if (verb < 99)
                    {
                        verb++;
                    }
                    else
                    {
                        verb = 0;
                        noun++;
                    }
                }
            }
        }

        destroy_intcode(prog);
    }
    return 0;
}
