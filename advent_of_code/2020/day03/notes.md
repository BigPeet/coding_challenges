For the first part it would be sufficient to read the input line-by-line and check if at the current index is a tree.
Then update the index (+ 3, % 31 (fixed-line-length)) and repeat with next line until all lines are processed.

Depending on what part 2 requests, it might be better to create a real "map", e.g. if backtracking becomes necessary.

Solution to the first part: 191.
