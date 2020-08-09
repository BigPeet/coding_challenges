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
#define DECK_SIZE 10007u

static const char* kCutCmd           = "cut ";
static const char* kNewStackCmd      = "deal into new stack";
static const char* kWithIncrementCmd = "deal with increment ";

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_22 FILE_PATH.\n");
        return 0;
    }

    FILE* fp = fopen(argv[1], "r");
    if (fp == NULL)
    {
        printf("Error when opening file %s\n", argv[1]);
        return 1;
    }

    /*Initialize our deck in 'factory' order*/
    Card deck[DECK_SIZE];
    for (size_t i = 0; i < DECK_SIZE; i++)
    {
        deck[i] = i;
    }

    char buffer[MAX_LINE_LENGTH];
    while (fgets(buffer, MAX_LINE_LENGTH, fp) != NULL)
    {
        if (strncmp(kCutCmd, buffer, strlen(kCutCmd)) == 0)
        {
            int64_t param = strtol(buffer + strlen(kCutCmd), NULL, 10);
            cut(deck, DECK_SIZE, param);
        }
        else if (strncmp(kNewStackCmd, buffer, strlen(kNewStackCmd)) == 0)
        {
            deal_into_new_stack(deck, DECK_SIZE);
        }
        else if (strncmp(kWithIncrementCmd, buffer, strlen(kWithIncrementCmd)) == 0)
        {
            int64_t param = strtol(buffer + strlen(kWithIncrementCmd), NULL, 10);
            deal_with_increment(deck, DECK_SIZE, param);
        }
    }

    if (DECK_SIZE == 10)
    {
        printf("Result: ");
        for (size_t i = 0; i < DECK_SIZE; i++)
        {
            printf("%u ", deck[i]);
        }
        printf("\n");
    }
    else
    {
        /*Get index of card 2019*/
        for (size_t i = 0; i < DECK_SIZE; i++)
        {
            if (deck[i] == 2019)
            {
                printf("Card 2019 is at index %zu\n", i);
                break;
            }
        }
    }


    if (!feof(fp))
    {
        printf("Didn't reach end of file.\n");
    }
    fclose(fp);
    return 0;
}
