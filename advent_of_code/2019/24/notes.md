This is basically a cellular automaton (with a variation of conway's game of life).

It looks quite straight-forward to implement with the biggest consideration currently towards storing the history of "scans".
I could either assume a maximum number of scans before a repitition is found and use a fixed-size array or create a dynamically growing container (list or vector).

Two things about the biodiversity:

1. The same biodiversity for two scans should mean both scans are identical => The biodiversity can be used to easy serizalize / de-serialize the scan. That might be easier to store.
2. If a scan is encoded as little-endian binary number (bugs = 1, empty = 0), the result should be the biodiversity.

The first solution was 17863741.

The question for part two is: When to add a new level in either direction.
And the answer should be: As soon as we need to add a bug there. Otherwise we can treat them simply as empty space.
I.e. if checking neighbouring cells from another level, if that level was not added yet, treat it as empty space.

In the end part two was also relatively easy, but somewhat hard to debug.
I could clean up / simplify the check_neighbour_02 function a little bit or separate the recursive calls into local functions, but the current version works.

The second solution was 2029.
