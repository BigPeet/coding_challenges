# Macros

Why use macros in C instead of either functions or global variables, etc.?

1. Macros are inlined => no function call overhead, useful for small functions
2. You don't have to specify types in a macro's definition.
    * `#define MAX(x,y) ((x) > (y) ? (x) : (y))` works with int, float and double
3. Macros can do "token pasting":
```c
#define DEF_PAIR_OF(dtype) \
typedef struct pair_of_##dtype { \
    dtype first; \
    dtype second; \
 } pair_of_##dtype##_t

DEF_PAIR_OF(int);
```
