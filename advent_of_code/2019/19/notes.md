Part 1 can be solved with a very minimalistic approach:

Start a copy of the intcode program, provide a coordinate as input and read the output.
Repeat.

You don't necessarily need to save the results to get the solution (yet).

The solution to the first part is 231.

For part two I have the following considerations:

* To fit 100x100 square (aligned to the grid), needs to be at least 100 points wide, but since it is angled, it needs up to 200 (depending on the exact angle).
* You can move along the beam when moving roughly along the (1,1) vector, but for greater distances a more accurate slope needs to be calculated.
    * This can be achieved by calculating the mid-point of the beam for each row and the fitting them into a line.
* The beam width increases roughly every 3 rows.

These considerations should help to find the approximate region for the 100x100 square.
Then you can check row-wise if a square would fit by checking its corner coordinates.
I think every possible candidate square's top-right corner would be positioned in the right-most field of the beam.
Alternatively checking the bottom right corner could eliminate a lot of checking.

After first calculations the slope of the beam seems to be ~1.22.

The solution for part two is 9210745.

The code could certainly be optimized by limiting the search space even more, e.g. calculating which beam width is needed for the square to fit and start your search there, but this simple approach works.
