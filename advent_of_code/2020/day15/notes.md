The solution for the first part is 694.

The naive solution for part two is to just let the program run for longer, i.e. for 30 million rounds.
When building with `--release`, this naive solution takes less than 2 seconds and gives the correct solution.

The solution for the second part is 21768614.

I searched on the internet and apparently, this naive solution is fine.
The "trick" would be that a linear search on a vector/list in part 1 would have worked, but would be too slow in part 2.
Since I've used a hashmap from the beginning, there was no real problem.
