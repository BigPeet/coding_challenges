The first step for this task is to get the communication with the robot going.

Mission Control sends command -> Robot receives command -> Robot sends output -> MC receives output.

This should be quite similar to the previous Intcode tasks, e.g. the painting robot from Day 11.
For this reason I will take the code of Day 11 as starting point for that (while using Day 13 version of the intcode).

The next step is to come up with an exploration strategy for the unknown space surrounding the robot.
The exploration strategy should prevent the robot from getting stuck and finding the oxygen system eventually.
Just following walls for example might not be good enough. Instead maybe something like this could be a first attempt:

1. Start with a random direction.
2. Follow direction until you hit a wall.
3. Locate nearest unknown field F.
4. Follow path to F. Repeat from Step 2.

This requires the ability to plan a path from the current robot position to any other field through known (already discovered) space.

Alternatively start with a "random" approach.

After the oxygen system is found, just search the shortest path from the starting field to the system.

This will require keeping track of:
* Position and direction or robot
* Keeping track of the board (saving discovered walls)
* Starting position
* resizing if necessary (updating the positions of walls and the start)

This path search also needs to explore though, since the robot might not have uncovered all the possible paths.
To explore the robot is needed.


I've decided to use a middle way between lazy random exploration and "sophisticated explorer".
If there is an unknown field, go there. If not, go to the field least visited.


Regarding the second part: This seems to be a "distance" question, since the room will be filled, when the field with the highest distance to the oxygen system is reached.
Luckily I built a distance map for part 1. So I should be able to just use the highest distance for part 2.

* First Solution: 232
* Second Solution: 320
