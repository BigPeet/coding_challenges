Part 1 can be solved relatively simple.

The solution for part 1 is 588200.

Part 2 requires some changes...it is infeasible to store the states of all these cubes.

But the following approach might work:

Start with a "mega-cuboid" `M`, i.e. a collection of cuboids.
For every reboot step:

* If it's ON, add its cuboid to `M`.
    * "Adding" means to only add the part which is not already represented in `M`.
    * E.g., if the new cuboid is completely overlapped by an existing, it is not added at all. If it doesn't overlap with any other cuboid in `M`, it is added as-is.
    * That might require splitting the added cuboid into multiple, smaller cuboids and add those selectively to `M`.
* If its OFF, the remove its cuboids from `M`.
    * "Removing" means to check if there is an overlap between this cuboid and any cuboid in `M`.
    * If so, then split the affected cuboid in such a way that the overlapped cuboid can be removed and the rest remain unaffected.

In the end, `M` only contains the ON cuboids.
So, iterate of the cuboids of `M` and count the cubes they contain (i.e. calculate their volume) and sum it all up.

The solution for part 2 is 1207167990362099.

My general approach was fine, but I had fought with an annoying off-by-one error in my cuboid-splitting.
I probably should have switch from inclusive-ranges to an easier, less error-prone modelling for the cuboids.
