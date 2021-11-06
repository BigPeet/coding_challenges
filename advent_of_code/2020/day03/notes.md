For the first part it would be sufficient to read the input line-by-line and check if at the current index is a tree.
Then update the index (+ 3, % 31 (fixed-line-length)) and repeat with next line until all lines are processed.

Depending on what part 2 requests, it might be better to create a real "map", e.g. if backtracking becomes necessary.

Solution to the first part: 191

Part 2 requires different "slopes" to take.
The simplest solution would be to simply parameterize the solution of the first part (i.e. the offset to the right and down).
Then apply that function to all 5 different slopes and calculate the puzzle answer.

That would require 5 separate passes over the input.
That should still be no problem, but an alternative approach would simply track the values for all 5 slopes in a single pass.

Solution to the second part: 1478615040
