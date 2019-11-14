#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

void read_input_numbers(const int argc, char** argv, int* input)
{
  for (int i = 1; i < argc; i++)
  {
    input[i - 1] = atoi(argv[i]);
  }
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

  if (amount_of_numbers == 2)
  {
    pair_t p = cons(numbers[0], numbers[1]);
    printf("Pair (%d, %d)\n", p.a, p.b);
    printf("car: %d\n", car(p));
    printf("cdr: %d\n", cdr(p));
    printf("car(cons(%d, %d,)) = %d\n", numbers[0], numbers[1], car(cons(numbers[0], numbers[1])));
    printf("cdr(cons(%d, %d,)) = %d\n", numbers[0], numbers[1], cdr(cons(numbers[0], numbers[1])));
  }

  return 0;
}
