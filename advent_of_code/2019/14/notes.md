Some initial thoughts on how to tackle this:

Every row of the input is a REACTION.
Every formula has a list of INPUTS and a single OUTPUT.
These are MATERIALS consisting of an amount and an identifier.

I need a (recursive) function `reduce_to_ore()` that takes any MATERIAL and translates it back to an amount of ORE.
Important to note that this needs to keep track of "spare" materials.
Take the example below:

```
5 ORE => 2 A
1 A, 2 ORE => 1 B
1 A, 1 B => 1 FUEL
```

If you would count the required ORE for the separate materials independent of each other, then you would come up with 7 ORE for 1 B and 5 ORE for 2 A, so 12 ORE for 1 FUEL and an extra A.

Instead you need to always keep in what your current STASH of material is.
Then you would process "1 B", come up with 7 ORE and a spare A.
When processing "1 A" next, you can just use the spare, for a total of 7 ORE for 1 FUEL.

If you process "1 A" first, you would come up with 5 ORE and a spare A.
Then when processing "1 B" you can again use the spare.

The function `reduce_to_ore()` should therefore have access to the following information:
* list of possible reactions (should not change)
* list of spare materials (a single list updated through the recursive calls)
    * might be an array initialized with every material (amount = 0)
* material (amount and name) to reduce to ORE

One difficulty that might come up is the efficient use of spare material.
There might be a situation, where using that spare A might save 5 ORE or 10 ORE depending on how it is used.
But for the start, I will assume that spare material should be used greedily.

If that does not work on the given examples, then a more complex strategy / search has to be done.
One solution for this could be to first calculate "cost" of each material independently and gather the spare material.
Then for each spare material check where it would reduce the cost the most.
This again might affect other spare material though (e.g. if that spare A is used for XYZ, then this spare B would not have been produced).
