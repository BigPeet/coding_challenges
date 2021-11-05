Ok, the naive solution is to take the first entry and compare with every other entry.
If no match was found, continue with the second entry, and so on...

That has a runtime of O(n^2), which might be fine for the first part.
But I suspect part 2 will be more difficult.

Another solution might involve sorting the list and then take the smallest and largest number.
If their sum is over 2020, replace the largest number with the 2nd largest number. Repeat.
If their sum is under 2020, replace the smallest number with the 2nd smallest number, etc.

I'm not sure, if that works, but if it does, then it should run in O(n) (well, O(nlog(n)), due to the sorting).

Solution for part 1: 902451

Well, as I've guessed the second part is a little more complex.
The naive approach would result in O(n^3).

While my chosen approach for part 1 can not be easily extended to 3 numbers, I can implement a "hybrid" approach:

Select the lowest value as fixed value x.
Then use the previous method to check if in the remaining range there are two values a, b for which a + b == 2020 - x.
If not, then increase x to the next highest value.
This should be in O(n^2), if I'm not mistaken.

Solution for part 2: 85555470
