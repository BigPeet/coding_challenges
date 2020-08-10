The first part seems relatively easy,
Implementing functions for `deal into new stack`, `cut` and `deal with increment` should be not too complicated.
Then read the input file line by line, parse the command and the parameter and execute the corresponding function.
I will make some *assumptions* on the input to ease the parsing, though.
Specifically, I will assume the parameters are valid (no cut with parameter larger than the deck, no string when an integer is expected) and that there is a maximum line width, so I can have a char-buffer on the stack.

First Solution: 6129

For part two: Since the actual parameters for the shuffle operations do not change, the potential for overflow is limited.
I'm actually more afraid of the stack / memory "overflowing".
If I wanted to store 119315717514047 cards, I would have to use at least 64bit per card to store the highest values.
That is simply too much memory.

I don't actually care about the whole deck, but only a single card (2019 in part one and 2020 in part two) and its index at the end.
Given the (hypothetical) deck size and the current index of the card, you can calculate where the card is going to be after an operation.
E.g. if the card is currently at index 1 and the deck has 10 cards, then after a `cut 2` it will at index 9.
So you can track a single card without keeping track of the others.

That solves the memory issue.

The next issue is how to shorten the amount of times we have to apply the shuffle.
Even if applying it once is relatively fast, applying it 101741582076661 times will be too slow.

The first naive approach for this would be to search for loops.
Let's say the shuffling algorithm repeats after 10000 applications.
Then I would know that the index after 10000, 20000, 30000, ... 101741582070000 applications is identical to the starting index.
Then only 6661 applications would remain.

A quick and dirty test indicates that this is probably not the way to go.

And now I read the task correctly and see that it asks what cards ends up at 2020...
I imagine I need to find a way to express the shuffle algorithm as an reversible function where the number of applications is simply an input parameter.
