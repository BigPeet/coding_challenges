The naive solution would be to find the minimal and maximal position in the input.
Then check the total fuel cost of every position.
This might work for part 1, but probably does not scale for part 2.

So instead we can just find the position with the least sum of all differences.
That should be the median.

The solution for the first part is 356179.

The second part sounds complex, but the target position should be at least close to the mean of all positions.

The solution for the second part is 99788435.
