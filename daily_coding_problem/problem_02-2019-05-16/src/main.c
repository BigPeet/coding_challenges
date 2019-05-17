#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

int* read_input_numbers(const int argc, char** argv, int input[argc])
{
    for (int i = 1; i < argc; i++)
    {
        input[i - 1] = atoi(argv[i]);
    }
    return input;
}

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        printf("Insufficient amount of arguments.\n");
        return 0;
    }

    int amount_of_numbers = argc - 1;
    int numbers[amount_of_numbers];
    read_input_numbers(argc, argv, numbers);

    printf("Number sequence: ");
    for (int i = 0; i < amount_of_numbers; i++)
    {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    int output[amount_of_numbers];
    process(numbers, amount_of_numbers, output);

    printf("Processed number sequence: ");
    for (int i = 0; i < amount_of_numbers; i++)
    {
        printf("%d ", output[i]);
    }
    printf("\n");


    return 0;
}
