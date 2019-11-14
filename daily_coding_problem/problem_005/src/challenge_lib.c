#include "challenge/challenge_lib.h"
#include "stddef.h"

static int pair(int_f f, int a, int b)
{
  return f(a, b);
}

static int fa(int a, int b)
{
  return a;
}

static int fb(int a, int b)
{
  return b;
}

pair_t cons(const int a, const int b)
{
  pair_t p;
  p.a    = a;
  p.b    = b;
  p.func = pair;
  return p;
}

int car(pair_t pr)
{
  return pr.func(fa, pr.a, pr.b);
}

int cdr(const pair_t pr)
{
  return pr.func(fb, pr.a, pr.b);
}
