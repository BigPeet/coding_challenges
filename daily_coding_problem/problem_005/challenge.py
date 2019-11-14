#!/usr/bin/env python3

# predefined
def cons(a, b):
    def pair(f):
        return f(a, b)
    return pair

def car(p):
    def f(a, b):
        return a
    return p(f)

def cdr(p):
    def f(a, b):
        return b
    return p(f)

if __name__ == "__main__":
    a = 3
    b = 4
    p = cons(3, 4)
    print(car(p))
    print(car(cons(3, 4)))
    print(cdr(p))
    print(cdr(cons(3, 4)))

