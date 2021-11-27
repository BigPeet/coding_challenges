This seems to be similar to Day 24 of AoC 2019.

One difference is not only the Z-dimension, but also the planes are limitless.
There is no fixed width or length.

One solution for this is to again use a map-like container, using the coordinate-tuple as key.
Whenever an active cell is inserted or a cell becomes active, all its neighbouring cells are inserted as empty (if they weren't contained in the map).

Then it should be sufficient to simply check all entries during every update step.

The solution for the first part is 375.
