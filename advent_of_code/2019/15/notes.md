The first step for this task is to get the communication with the robot going.

Mission Control sends command -> Robot receives command -> Robot sends output -> MC receives output.

This should be quite similar to the previous Intcode tasks, e.g. the painting robot from Day 11.

The next step is to come up with an exploration strategy for the unknown space surrounding the robot.
Just following walls for example might not be good enough. Instead maybe something like this could be a first attempt:

1. Start with a random direction.
2. Follow direction until you hit a wall.
3. Locate nearest unknown field F.
4. Follow path to F. Repeat from Step 2.

This exploration strategy should prevent the robot from getting stuck and finding the oxygen system eventually.
It requires planning the ability to plan a path from the current robot position to any other field through known (already discovered) space.

After the oxygen system is found, just plan the shortest path from the starting field to the system.

This will require keeping track of:
* Position and direction or robot
* Keeping track of the board (saving discovered walls)
* Starting position
* resizing if necessary (updating the positions of walls and the start)
