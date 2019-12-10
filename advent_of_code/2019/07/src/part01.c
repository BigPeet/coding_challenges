/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-09
 *
 */

#include "challenge/challenge_lib.h"
#include "challenge/intcode.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

#define BUFFER_LENGTH (size_t)(32)

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        printf("This executabel takes exactly three arguments.\n");
        printf("Usage: aoc2019_07 FILE_PATH INPUT_FILE OUTPUT_FILE.\n");
        return 0;
    }

    char* infile  = argv[2];
    char* outfile = argv[3];


    size_t num_amplifiers = 5;
    intcode_t* prog       = read_intcode(argv[1]);

    if (prog == NULL)
    {
        return 0;
    }

    size_t num_perms = 0;
    int** perms      = create_permutations(num_amplifiers, &num_perms);
    if (perms == NULL)
    {
        return 0;
    }

    int max_output = -1; /*should be min-int*/

    for (int j = 0; j < num_perms; j++)
    {
        int input   = 0; /*as described by task*/
        int* phases = perms[j];

        for (size_t i = 0; i < num_amplifiers; i++)
        {
            /*Write new input for next programm*/
            FILE* ifp = fopen(infile, "w");
            if (ifp != NULL)
            {
                char buffer[BUFFER_LENGTH];
                sprintf(buffer, "%d\n", phases[i]);
                fputs(buffer, ifp);
                sprintf(buffer, "%d\n", input);
                fputs(buffer, ifp);
                fclose(ifp);
            }

            /*Create copy*/
            intcode_t* copy = copy_intcode(prog);
            if (copy == NULL)
            {
                return 0;
            }
            set_io_mode(copy, INT_CODE_STD_IO);

            /*Redirecting stdin and stdou to files*/
            freopen(infile, "r", stdin);
            freopen(outfile, "w", stdout);
            int ret = execute(copy);
            destroy_intcode(copy);

            fclose(stdin);
            fclose(stdout);

            if (ret == INT_CODE_HALT)
            {
                /*Read output of amplifier into input for next one*/
                FILE* ofp = fopen(outfile, "r");
                if (ofp != NULL)
                {
                    if (fscanf(ofp, "%d", &input) != 1)
                    {
                        printf("No output!\n");
                        return 1;
                    }
                    fclose(ofp);
                }
            }
            else
            {
                printf("Programm did not halt as expected. Err code: %d\n", ret);
                break;
            }
        }
        if (max_output < input)
        {
            max_output = input;
        }
    }

    freopen(outfile, "w", stdout);
    printf("Max output: %d\n", max_output);
    fclose(stdout);

    destroy_permutations(perms, num_perms);
    destroy_intcode(prog);
    return 0;
}
