/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2020-08-10
 *
 */

#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define MAX_LINE_LENGTH 255u
#define DECK_SIZE 119315717514047u
#define LOOPS 101741582076661u


static const char* kCutCmd           = "cut ";
static const char* kNewStackCmd      = "deal into new stack";
static const char* kWithIncrementCmd = "deal with increment ";

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executable takes exactly one argument.\n");
        printf("Usage: aoc2019_22 FILE_PATH.\n");
        return 0;
    }

    uint64_t index = 2020u;


    for (uint64_t shuffle_reps = 0; shuffle_reps < LOOPS; shuffle_reps++)
    {
        FILE* fp = fopen(argv[1], "r");
        if (fp == NULL)
        {
            printf("Error when opening file %s\n", argv[1]);
            return 1;
        }


        char buffer[MAX_LINE_LENGTH];
        while (fgets(buffer, MAX_LINE_LENGTH, fp) != NULL)
        {
            if (strncmp(kCutCmd, buffer, strlen(kCutCmd)) == 0)
            {
                int64_t param = strtol(buffer + strlen(kCutCmd), NULL, 10);
                cut_index(&index, DECK_SIZE, param);
            }
            else if (strncmp(kNewStackCmd, buffer, strlen(kNewStackCmd)) == 0)
            {
                deal_into_new_stack_index(&index, DECK_SIZE);
            }
            else if (strncmp(kWithIncrementCmd, buffer, strlen(kWithIncrementCmd)) == 0)
            {
                int64_t param = strtol(buffer + strlen(kWithIncrementCmd), NULL, 10);
                deal_with_increment_index(&index, DECK_SIZE, param);
            }
        }
        fclose(fp);
    }
    printf("The card is at index %zu\n", index);
    return 0;
}
