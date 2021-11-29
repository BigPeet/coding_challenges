The solution for part 1 is 178.
But I don't like the solution I ended up with for part 1.
I basically built a collection of a valid strings for rule 0.
That is a lot of senseless work and it won't work for part 2.

The better approach would be to "match" the strings against the rule and see if it is valid.
This would basically allow to traverse the rule "tree" as far as needed and allow for an early abort.
