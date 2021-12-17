I tailored my solution to only work with target areas which are in a single bottom quadrant.
It can be left or right, but it has to be below the origin.
It can not cross the x or y axis.

Lets say x2 is the x-coordinate of the area farther away from the origin and x1 is the closer one.
Ignoring the y-dimension for now, the x of a "valid" velocity must be between
* the minimal value for which this is true: `x_min + x_min - 1 + x_min - 2 + ... + 0 >= x1`
* and x2

For y a similar range can be created.
The minimal value for y is the lower y coordinate of the target area. Aiming lower than the target will always miss.

The maximal sensible value for y is the lower y coordinate times -1 (assuming the area is in the bottom quadrant).
For any positive y value, the probe will come back to the x-axis (y=0) with a y-value velocity of -1 * time the initial y.

If that is greater than the absolute value of the lower bound, it will always skip the target.

The solution for the first part is 9870.

Part two was basically already covered with the implementation for part 1.

The solution for the second part is 5523.
