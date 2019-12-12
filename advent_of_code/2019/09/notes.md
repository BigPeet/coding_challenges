This task builds upon the "intcode" implementation from Day 2, Day 5 and Day 7.
For this reason, I've copied the Day 7 "intcode" files as starting point for this Task.

Steps:
  * ~Change the memory from `int` to `int64_t` to support large numbers.~
    * ~maybe even more is necessary~
    * ~is tested with test\_input\_03.txt (and some unit tests later)~
  * ~Add new parameter mode (4) for relative parameters.~
    * ~can be used to store~
    * ~need relative base~
  * ~Add operator code (9) for setting relative base~
  * ~Add additional memory to the program.~
    * ~Start with the normal size, but if memory outside the current size is accessed, realloc the memory to the required size.~

* First Solution: 2377080455
* Second Solution: 74917


I have no idea what the issue/challenge for part 2 was...I just entered "2" and the output came a few milliseconds later.
