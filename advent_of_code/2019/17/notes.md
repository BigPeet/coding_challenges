Of course I will reuse the intcode programm from the previous days.

As far as I understand, during the first part, only output in form of ASCII coded characters is provided.
I would assume that a bunch of initial output is given to build up the initial frame.
Without running it, I don't know if the "camera" will endlessly spam output, continue to run without outputting or HALT.
If it continues to run, how to decide when to stop? Time-based?
*It halts in the initial mode.*

The output can be fed into an "area" (similar to previous robot tasks), but this time the coordinates are more implied, e.g. via linebreaks (10).
I will proceed similarly to previous tasks: start with small starting size and increase the size based on the given output.

After parsing the output into the overview of the "area", locating intersecting scaffolds is the next step.
That should be easy and after locating them in the "area" (with X,Y-positions) getting the alignment parameters should also be done easily.

* First Solution: 6244

Part two looks like a real cool challenge.

I would break it down into the following tasks:

1. Find a (minimal) path "String" that visits every scaffold tile at least once.
    * Simple solution: Find it by hand based on the given camera input and hardcode the string.
      I might start with this to check if the resulting path can be used in Step 2. If not, I might have to change my algorithm.
    * ~To find a "good" path I would use the intersections from Part 1.~
    * ~Something like:~
        * ~At start: Find path to closest (unvisited) intersection and follow it.~
        * ~At intersection: check fields of directions you have not been coming from, take the path/direction with the most unvisited fields to an (preferably) unvisited intersection.~
        * ~Only enter dead ends, if there are more than two unvisited dead ends left.~
        * ~Only enter the last dead end, if all fields outside of that dead end have been visited.~
        * ~A field counts as visited, when the leaving the field.~
    * ~For this I need:~
        * ~way to keep track of visited fields (by the search algorithm not the robot)~
        * ~method to find dead ends on the map.~
        * ~method to count the number of visited fields~
        * ~method to check is-intersection or is-dead-end for a given Position~
        * ~method to find the next "end position" of a path, given current position and direction. "end points" are either dead ends or intersections.~
            * ~it should also return the number of unvisited fields on the path~
        * ~Paths should contain a string with their representation (e.g. R,8,L), starting and end pos~
            * ~This way the visited path can be appended to the total path during planning.~
    * I way overthought this last night. It is apparently sufficient to just follow the current direction for as long as possible, until you would fall off.
    * Then turn either left or right (whichever is possible).
    * Stop when hitting a dead end (I assume there is always only one dead end besides the starting point).
    * That will in a sufficient path.

2. Divide this path fully into three repeating sub-strings (sub-paths), each with at most 20 characters (including ',').
    * These sub-strings become the movement functions.
    * The order of these sub-strings in the original string is the main movement routine.
    * For this you need to find all "repeating" patterns (with sizes 1 to at most 20 characters) in the original string.
    * Then find a combination of three of these patterns that cover the whole original string.
    * If the resulting main movement routine is too large, continue searching
    * An open question is, if any path can be divided into three viable "sub-routines". Or if this has to be something taken into account during the creation of the path.
        * I might check this with some "manual" paths.
        * ~One possible consideration: If there are two (or three) equally good candidates at an intersection, take the one that does not require turning.~

Then these inputs have to be provided to the robot and the final output has to be read.

As there were many open questions regarding how to find a decent path, etc., I started with an quick&dirty implementation in Python instead of C to verify or correct my assumptions.
Using the python script "part02_path_stuff.py" I found a solution for a path I've manually crafted using the algorithm described above.

* Chosen path: L,12,L,8,L,8,L,12,R,4,L,12,R,6,L,12,L,8,L,8,R,4,L,12,L,12,R,6,L,12,R,4,L,12,R,6,L,12,L,8,L,8,R,4,L,12,L,12,R,6,L,12,L,8,L,8,R,4,L,12,L,12,R,6,L,12,R,4,L,12,R,6
* A: L,12,L,8,L,8
* B: L,12,R,4,L,12,R,6
* C: R,4,L,12,L,12,R,6
* Sequence:  A,B,A,C,B,A,C,A,C,B

For now I will just use these found solutions in the C program to check if it will solve the challenge.
If I feel motivated, I will also implement the "path generation" and reimplement the "dividing step" in C, so the C code can handle different inputs.
But due to running low on spare time, I will likely move on to Day 18 instead.
