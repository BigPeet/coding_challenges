Apparently the numbers are all single-digits.
This might allow for easier parsing, as it enables evaluating the lines char-by-char.

Alternatively, the lines are split by whitespace and then evaluated "token"-wise.
For this to work with the input, whitespaces need to be added around '(' and ')'.

The solution for the first part is 12956356593940.

For the second solution I moved the parsing process into a separate type.
I implemented that as a trait, but in the end the changing behavior basically boiled down to a different parameter.
I've kept the trait implementation anyway.

The solution for the second part is 94240043727614.
