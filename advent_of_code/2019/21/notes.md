Since this will make use of the intcode and ASCII logic, I will use the code of day 17 as initialization.

A few first thoughts on part 1:

* We have to make our decision based on four booleans, giving us up to 16 possible scenarios.
* But we can reduce this quickly by assuming that we always have to jump, when the next tile is a hole.
* We can further reduce this because we shouldn't jump if a hole is 4 tiles ahead of us (since we would jump right into it).
* First naive solution: Jump when there is a hole in the next three tiles (A, B, C), but only if there is no hole in D (where we would land).

This first naive solution worked for Part 1.

Solution: 19349939

For Part 2 we need to expand the solution to also check the tiles farther out.
Apply the same conditions as before and in addition don't jump, if that leads you into a position where you can't avoid a hole anymore, e.g.:
* If E has a hole, then we would need to immediately jump again. But if H has a hole too, then we fall into it. So, if possible we need to delay our jump.
To fit this in as few instructions as possible:
First make the check if we should jump early (check B and C), then if we can safely (by checking D and H).
Then OR this with a definitive jump, if A has a hole.

Solution: 1142412777
