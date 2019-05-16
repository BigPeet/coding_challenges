#include "stdio.h"
#include "stdlib.h"
#include "stdbool.h"
#include "problem/list_check.h"

int* read_input_numbers(const int argc, char** argv, int* k, int input[argc])
{
    *k = atoi(argv[1]);
    for (int i = 2; i < argc; i++)
    {
        input[i - 2] = atoi(argv[i]);
    }
    return input;
}

int main(int argc, char* argv[])
{
    if (argc < 3)
    {
        printf("Insufficient amount of arguments. Enter K followed by at least "
               "1 number\n");
        return 0;
    }

    int k                 = 0;
    int amount_of_numbers = argc - 2;
    int numbers[amount_of_numbers];
    read_input_numbers(argc, argv, &k, numbers);
    printf("K = %d\n", k);
    printf("Number sequence: ");
    for (int i = 0; i < amount_of_numbers; i++)
    {
        printf("%d ", numbers[i]);
    }
    printf("\n");

    bool check = contains_summands(k, amount_of_numbers, numbers);

    printf("Contains Summands for K: %d\n", check);

    return 0;
}
