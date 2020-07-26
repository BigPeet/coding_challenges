/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        printf("This executabel takes exactly two arguments.\n");
        printf("Usage: aoc2019_16 FILE_PATH PHASES.\n");
        return 0;
    }

    int phases    = atoi(argv[2]);
    Sequence* seq = read_sequence(argv[1]);
    if (seq == NULL)
    {
        return 0;
    }

    /*Part 01*/
    Sequence* result = run_phases(seq, phases, 0);
    if (result != NULL)
    {
        printf("Part 01: ");
        print_sequence(result, 8);
        destroy_sequence(result);
    }

    /*Part 02*/
    int offset_value         = 0;
    Sequence* message_offset = get_subsequence(seq, 0, 7);
    if (message_offset != NULL)
    {
        offset_value = get_value(message_offset);
        destroy_sequence(message_offset);
    }
    printf("Message offset: %d\n", offset_value);


    Sequence* repeated = repeat_sequence(seq, 10000);

    /*Since only the second half of the output is relevant,*/
    /*I can skip the expensive calulations of the first half.*/
    int skip_first_half = offset_value > (repeated->size / 2);
    result              = run_phases(repeated, phases, skip_first_half);
    destroy_sequence(repeated);


    if (result != NULL)
    {
        Sequence* solution = get_subsequence(result, offset_value, 8);
        if (solution != NULL)
        {
            printf("Part 02: ");
            print_sequence(solution, solution->size);
            destroy_sequence(solution);
        }
        destroy_sequence(result);
    }


    destroy_sequence(seq);

    return 0;
}
