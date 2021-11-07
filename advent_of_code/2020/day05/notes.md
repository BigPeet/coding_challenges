So this is a simple binary encoding.
Take the seat specification, for the seven characters specifiying the row, interpret F as 0 and B as 1.
For the column, interpret L as 0 and R as 1.

FBFBBFFRLR becomes two binary numbers:

0101100 (= 44) and 101 (= 5).

Solution for the first part: 913

For the second part the naive solution is:

Iterate over all possible seats (or IDs), check if the ID is in the parsed list. If not, check if the +/-1 IDs are in the list.
If searching is done in O(n), the this would be in O(n^2).

Using a data structure with O(1) search/access could be an improvement, lowering it to O(n) overall.

Solution for the second part: 717
