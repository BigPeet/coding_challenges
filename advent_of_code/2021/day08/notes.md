The first part is relatively easy and likely just supposed to setup part 2.
I will just provide a relatively minimal solution for part 1 and then build it up as necessary for part 2.

The solution for the first part is 504.

One algorithm to deduce the remaining digits goes as follows:

* I will refer to the display segments as top, top-left, top-right, middle, bottom-left, bottom-right and bottom.
* 1, 4, 7 and 8 are the easy digits and can be identified.
* 1 will give you two values (a, b in the example) which could be either top-right or bottom-right.
* Look at all digits with 6 segments (i.e. 0, 6 and 9). Which one of those does is missing one of the values above? That is 6.
    * This clearly identifies top-right and bottom-right. (2 down)
* Look at 7.
    * Its third still unknown value is top value (3 down).
* Look at the remaining digits with 6 segments (i.e. 0 and 9). Which one of those is missing one of the values from 4? That is 0.
    * The missing value is middle (4 down).
* Look at 4.
    * With top-right, bottom-right and middle known, the last unknown value of 4 is top-left (5 down).
* Look at 0 and 9. Which value is 9 missing which is present in 0 which is not the known middle?
    * That is the bottom-left (6 down).
    * The last unknown value is automatically bottom (7 down).

My implementation of that algorithm is somewhat messy (it was late :-/).

The actual calculations with the deduced "wiring" is relatively easy.

The solution for the second part is 1073431.
