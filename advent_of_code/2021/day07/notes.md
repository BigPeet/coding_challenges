The naive solution would be to find the minimal and maximal position in the input.
Then check the total fuel cost of every position.
This might work for part 1, but probably does not scale for part 2.

So instead we can just find the position with the least difference to all values.
That should be the median.

The solution for the first part is 356179.
