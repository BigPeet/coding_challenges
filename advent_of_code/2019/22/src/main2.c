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
#define INSTRUCTION_SIZE 100u
#define DECK_SIZE 119315717514047u
#define LOOPS 101741582076661u


static const char* const kCutCmd           = "cut ";
static const char* const kNewStackCmd      = "deal into new stack";
static const char* const kWithIncrementCmd = "deal with increment ";

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executable takes exactly one argument.\n");
        printf("Usage: aoc2019_22 FILE_PATH.\n");
        return 0;
    }

    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("Error when opening file %s\n", argv[1]);
        return 1;
    }

    uint64_t index = 2020u;
    uint64_t card  = 0u;
    Instruction instructions[INSTRUCTION_SIZE];
    uint64_t ic = 0u;
    char buffer[MAX_LINE_LENGTH];

    // Parse instructions
    while (fgets(buffer, MAX_LINE_LENGTH, fp) != NULL)
    {
        if (strncmp(kCutCmd, buffer, strlen(kCutCmd)) == 0)
        {
            int64_t param      = strtol(buffer + strlen(kCutCmd), NULL, 10);
            Instruction cut    = {Cut, param};
            instructions[ic++] = cut;
        }
        else if (strncmp(kNewStackCmd, buffer, strlen(kNewStackCmd)) == 0)
        {
            Instruction deal_new = {DealIntoNew, -1};
            instructions[ic++]   = deal_new;
        }
        else if (strncmp(kWithIncrementCmd, buffer, strlen(kWithIncrementCmd)) == 0)
        {
            int64_t param         = strtol(buffer + strlen(kWithIncrementCmd), NULL, 10);
            Instruction deal_with = {DealWithIncrement, param};
            instructions[ic++]    = deal_with;
        }
    }
    fclose(fp);

    card = no_in_position_after(instructions, INSTRUCTION_SIZE, index, DECK_SIZE, LOOPS);

    printf("The card at index %zu is %zu\n", index, card);
    return 0;
}
