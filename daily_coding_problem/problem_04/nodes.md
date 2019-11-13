This was a tricky task.

I started off with the imperfect solutions to get a better grip on the problem
They also helped me to setup test cases for the development of the proper solution.

I got stuck for a while: I knew that sorting the array would make it trivial to find the solution, but I got stuck on the O(n log n) limit for comparison based sorts.
I did not even implement such a "slow" sort-based solution.
At some point I finally understood that if I have only values in \[0, n\] (with n being the number of elements),
I could just put every number "where it belonged" (i.e. I put "1" to at index 0, "2" at index 1, etc. while skipping "0").
