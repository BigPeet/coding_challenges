The first part seems relatively easy,
Implementing functions for `deal into new stack`, `cut` and `deal with increment` should be not too complicated.
Then read the input file line by line, parse the command and the parameter and execute the corresponding function.
I will make some *assumptions* on the input to ease the parsing, though.
Specifically, I will assume the parameters are valid (no cut with parameter larger than the deck, no string when an integer is expected) and that there is a maximum line width, so I can have a char-buffer on the stack.

First Solution: 6129
