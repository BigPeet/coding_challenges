Since this task is very similar to Day 18 I decided to continue using the Python implementation.

Part 1 at least could be easily solved by using BFS.
The solution is 570.

Part 2 needs a distinction between maze levels.
Every portal pair has an outer and an inner portal that decreases or increases the level.

While on level 0 the outer portals can't be used and their fields are treated as walls, except for AA and ZZ.
On all other levels AA and ZZ are treated as walls.

On a first glance this should not really affect how we calculate the minimal distance, just how we traverse the maze.
Therefore the prior solution only needs updating for handling the "portal logic".

The solution for part 2 is 7056.
