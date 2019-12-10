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
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_07 FILE_PATH INPUT_FILE OUTPUT_FILE.\n");
        return 0;
    }

    char* infile  = argv[2];
    char* outfile = argv[3];

    size_t num_amplifiers = 5;
    size_t offset         = 5;

    intcode_t* amplifier_progs[num_amplifiers];
    intcode_t* prog = read_intcode(argv[1]);
    if (prog == NULL)
    {
        return 0;
    }
    for (size_t i = 0; i < num_amplifiers; ++i)
    {
        /*Create copy*/
        intcode_t* copy = copy_intcode(prog);
        if (copy == NULL)
        {
            return 0;
        }
        amplifier_progs[i] = copy;
    }

    size_t num_perms = 0;
    int** perms      = create_permutations_with_offset(num_amplifiers, offset, &num_perms);
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
            intcode_t* amp_prog = amplifier_progs[i];
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


            /*Redirecting stdin and stdou to files*/
            freopen(infile, "r", stdin);
            freopen(outfile, "w", stdout);
            int ret = execute(amp_prog);
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

    for (size_t i = 0; i < num_amplifiers; ++i)
    {
        destroy_intcode(amplifier_progs[i]);
    }

    destroy_permutations(perms, num_perms);
    destroy_intcode(prog);
    return 0;
}
