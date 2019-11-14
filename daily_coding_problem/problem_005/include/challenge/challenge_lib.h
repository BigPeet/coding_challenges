#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

typedef int (*int_f) (int, int);
typedef int (*pair_f) (int_f, int, int);

typedef struct pair_t
{
  int a;
  int b;
  pair_f func;
} pair_t;

pair_t cons(const int a, const int b);
int car(const pair_t pr);
int cdr(const pair_t pr);


#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
