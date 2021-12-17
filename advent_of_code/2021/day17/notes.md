Lets say x2 is the x-coordinate of the area farther away from the origin and x1 is the closer one.
Ignoring the y-dimension for now, the x of a "valid" velocity must be between
* the minimal value for which this is true: `x_min + x_min - 1 + x_min - 2 + ... + 0 >= x1`
* and x2

For any X outside this range, the probe will always miss.

So for any x inside that range, which "step" lies within the target area.
For these steps, probe the y-values starting with the area's lower bound.

For some y-value the "shot" will start hitting the area.
Increasing the y-value further, it will start missing again.
At that point, higher y-values don't need to be considered.
