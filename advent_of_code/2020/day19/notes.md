The solution for part 1 is 178.
But I don't like the solution I ended up with for part 1.
I basically built a collection of a valid strings for rule 0.
That is a lot of senseless work and it won't work for part 2.

The better approach would be to "match" the strings against the rule and see if it is valid.
This would basically allow to traverse the rule "tree" as far as needed and allow for an early abort.

So to start, I've updated my implementation for part 1 to use this improved approach.

I've "simplified" the Rule type again to use simply the indeces.
Then I implemented a "hardcoded" cycle workaround.
Will only work for this very specific rule set (as hinted at in the task).

The solution for part 2 is 346.
