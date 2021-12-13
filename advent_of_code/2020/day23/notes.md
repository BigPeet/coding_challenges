The naive implementation of this task requires splitting and merging the "collection" of cups at "random" indeces.
A linked-list is theoretically suited best for this, even though I think a vector could still result in better performance.

Nonetheless I will use a LinkedList just to have used it once.

The solution for the first part is 89573246.

For the second part the number of cups is relatively large, so it is unwise to perform the splitting operation (which is O(n)) multiple times per round.
Other operations in O(n) are also not helpful, e.g. Vec::rotate\_left.

To get away from the O(n) operations, I can create a specialized "linked-list-map" with O(1) look up.
This can be achieved by using a vector, which stores the next cup for each cup at the cup's index.
I.e. v[cup] == next\_cup
Then every move only the few affected cups need to be updated (again in constant time).

The solution for the second part is 2029056128.
