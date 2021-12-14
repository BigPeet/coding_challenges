The naive implementation should likely work for part 1.
But this looks like it might "explode", when run for longer.
I will move ahead with the naive implementation until the specifics of part 2 are known.

The solution for the first part is 3048.

As I already thought, the second part is simply to let this process run 40 steps instead of 10.

The slow part is iterating and inserting over the growing list of characters in the polymer.

What is needed instead is a constant time update, ot at least an update time independent from the length of the polymer.

What remains constant throughout the whole process is the set of characters (possibly A to Z) and set of insertion rules.

So my initial idea would be:

* Count the initial occurrences of each character and store them into an array (occ[0; 26]).
* Setup a dictionary that can be indexed by the left-hand side pairs of the insertion rules and one additional character.
* Then count how often these pairs and additional character occurr in the initial sequence.
    * E.g., in the given example `pair[('NN', 'C')]` (or simply `pair['NNC']`) would be initialized to 1, `pair['NNA']` would be initialized to 0, etc.
    * A special case is introduced for the last pair. It will be entered as `pair['CB_']`.
* Now during an update step, these counts have to be updated.
    * Create a new pair-dictionary. (necessary???)
    * Iterate over the pair-dictionary. For every three-char-sequence of 'xyz':
        ```
        let occ = pair[xyz]
        let a = rule[xy]
        let b = rule[yz].or(_)
        counts[a] += occ
        new_pair[xay] += occ
        new_pair[ayb] += occ
        ```
    * Finally, replace the old pair with the new pair dictionary.
    * This only really works, if there is a rule for every pair which can occurr. Otherwise, that case would need to be handled.
        * But I assume that holds true for the input.

The solution for the second part is 3288891573057.
