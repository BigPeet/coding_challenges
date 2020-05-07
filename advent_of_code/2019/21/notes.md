Since this will make use of the intcode and ASCII logic, I will use the code of day 17 as initialization.

A few first thoughts on part 1:

* We have to make our decision based on four booleans, giving us up to 16 possible scenarios.
* But we can reduce this quickly by assuming that we always have to jump, when the next tile is a hole.
* We can further reduce this because we shouldn't jump if a hole is 4 tiles ahead of us (since we would jump right into it).
