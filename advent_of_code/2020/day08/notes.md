This task is similar to last year's intcode, with some changes, of course.

The solution for the first part is 1087.

The naive solution for part 2 is to brute force and test every nop and jmp instruction until the solution is found.
Alternatively, checking where how the loop is entered and analysing possible changes might also work.
* Find the loop entry point.
* Step back until finding a JMP/NOP.
* Create a copy of the program with this instruction switched.
* Does it terminate successfully?
    * Yes: Done
    * No: Take the original program and step back further into the instruction history. Repeat.

The solution for the second part is 780.
