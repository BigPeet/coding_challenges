The first part should be easily solvable by sorting the list first.

The solution for the first part is 2590.

Part two seems more difficult. If there are trillions of options, it would be unwise to just test all of them.

My first idea is to use the differences of 3 as hard separators.
These connections HAVE to be selected and separate the whole chain into smaller chunks.
For each chunk the possible valid combinations can be calculated, then multiply the results of all chunks to get the number of all possibile combinations.

Addendum:
The start and end adapter of each range HAVE to be enabled.
To get the number of all possible combinations of a chunk, take the number of all combinations for the inner adapters and substract the number of invalid combinations.
If the difference between start and end is less than or equal to 3, then there are no invalid combinations.

Example 1:

Given the range [1,2,3,4]:
1 and 4 have to be enabled (1), while 2 and 3 can either be enabled (1) or disabled (0).
This means there are 2^2 possible combinations and no further invalid combinations.

Example 2:
Given the range [1,2,3,4,5]:

Again, 1 and 5 have to be set. 2, 3 and 4 can either be enabled (1) or disabled (0).
So there are 2^3 possible combinations in total.
But the combination of 2, 3 and 4 being disabled is invalid.
So the combinations for this range are 2^3 - 1.
The largest ranges in the input have this size.

Example 3:
Given the range [1,2,3,4,5,6,7]:
For this range there are a total of 2^5 = 32 possible combinations.
To evaluate how many invalid combinations a larger range has, consider each 3-block separately first:
If the block 2,3,4 is disabled, then the combination is invalid regardless of the values of adapters 5 or 6.
So for this block, there are 2^2 invalid combinations, with (5,6) being either (0,0), (0,1), (1,0) or (1,1).
The same is true for the block 3,4,5 and 4,5,6.
This would lead to 3 * 2^2 = 12 invalid combinations.
But some of these 12 combinations are duplicates. E.g. the combination with 2 through 6 being disabled is counted 3 times.
See the combinations here:

```
Block 2,3,4
 1  2 3 4 5 6  7
(1) 0 0 0 0 0 (1)
(1) 0 0 0 0 1 (1)
(1) 0 0 0 1 0 (1)
(1) 0 0 0 1 1 (1)

Block 3,4,5
 1  2 3 4 5 6  7
(1) 0 0 0 0 0 (1) => duplicate
(1) 0 0 0 0 1 (1) => duplicate
(1) 1 0 0 0 0 (1)
(1) 1 0 0 0 1 (1)

Block 4,5,6
 1  2 3 4 5 6  7
(1) 0 0 0 0 0 (1) => duplicate
(1) 0 1 0 0 0 (1)
(1) 1 0 0 0 0 (1) => duplicate
(1) 1 1 0 0 0 (1)
```

So the actual number of invalid combinations is 3 * 2^2 - 2 * 2^1 = 8 leading us to a total of 32 - 8 = 24 valid combinations.


Given all these examples, the following formula can be deduced (not proven):

```
let l = size of range (e.g. 7 in example 3)
let k = l - 2 (i.e. length of the inner range)
let m = k - 2 (i.e. the number of 3-blocks)
```

Then the number of valid combinations for each chunk is:

```
let total_combinations = 2^k
let invalid_combinations = m * 2^(m-1) - (m-1) * 2^(m-2)
let valid_combinations = total_combinations - invalid_combinations
```



The solution for the second part is 226775649501184.
