Of course I will reuse the intcode programm from the previous days.

As far as I understand, during the first part, only output in form of ASCII coded characters is provided.
I would assume that a bunch of initial output is given to build up the initial frame.
Without running it, I don't know if the "camera" will endlessly spam output, continue to run without outputting or HALT.
If it continues to run, how to decide when to stop? Time-based?

The output can be fed into an "area" (similar to previous robot tasks), but this time the coordinates are more implied, e.g. via linebreaks (10).
I will proceed similarly to previous tasks: start with small starting size and increase the size based on the given output.

After parsing the output into the overview of the "area", locating intersecting scaffolds is the next step.
That should be easy and after locating them in the "area" (with X,Y-positions) getting the alignment parameters should also be done easily.
