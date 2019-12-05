#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_05 FILE_PATH.\n");
        return 0;
    }

    scanf("TEST: \n");

    /*intcode_t* prog = read_intcode(argv[1]);*/
    /*if (prog != NULL)*/
    /*{*/
        /*print_intcode(prog);*/

        /*[>Create copy<]*/
        /*intcode_t* copy = copy_intcode(prog);*/
        /*if (copy != NULL)*/
        /*{*/
            /*int ret = execute(copy);*/
            /*switch (ret)*/
            /*{*/
                /*case 0:*/
                /*case -1:*/
                    /*printf("Something went wrong!\n");*/
                    /*break;*/
                /*case 1:*/
                    /*printf("We reached 99.\n");*/
                    /*print_intcode(copy);*/
                    /*printf("Output of program: %d\n", output_intcode(copy));*/
                    /*break;*/
                /*case 2:*/
                    /*printf("We ran out of stuff??\n");*/
                    /*break;*/
                /*default:*/
                    /*printf("Default: Something went wrong!\n");*/
                    /*break;*/
            /*}*/
            /*destroy_intcode(copy);*/
        /*}*/
        /*destroy_intcode(prog);*/
    /*}*/
    return 0;
}
