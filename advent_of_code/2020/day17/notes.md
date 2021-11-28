This seems to be similar to Day 24 of AoC 2019.

One difference is not only the Z-dimension, but also the planes are limitless.
There is no fixed width or length.

One solution for this is to again use a map-like container, using the coordinate-tuple as key.
Whenever an active cell is inserted or a cell becomes active, all its neighbouring cells are inserted as empty (if they weren't contained in the map).

Then it should be sufficient to simply check all entries during every update step.

The solution for the first part is 375.

With my implementation adding a dimension is relatively trivial.
I could just update the existing types and be done with it.
But instead I want to make the solution more generic, so both parts can be handled at the same time.

The solution for the second part is 2192.

I wanted to make the coordinate type more generic, but sadly there don't seem to be non-type "template parameter" in Rust.
So I couln't just make something like Coordinate<4>.
But I was able to implement a trait, which defines the interface for the CubeMap, so no duplication for the CubeMap's code was needed there.

There is still more duplicated code than I would like (for Coordinate3d, Coordinate4d and their neighbour iterators).
I might look into consolidating that as well.

I ended up creating a single "neighbour" iterator which can work for 3 and 4 dimensions.
Overall the code is a little bit slower though.
I suspect the additional division and modulo operations are to blame.

Update: Removed the division and modulo operations by creating all "deltas" ahead of time instead.
With that change, the solution is as fast as before while still using a single implementation for the neighbour iterator.
