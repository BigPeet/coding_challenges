Since I was not able to do these every day, I fell somewhat behind.
I guess this tasks is a "one-of" task (i.e. the code will not really be re-used later on), I will do this a little more "quick-and-dirty".
For example, I will provide the dimension of the input map to the program instead of parsing that dynamically.

It took me a while to fully understand the blocking rules. I did not think about "reducing" the increments (see F blocks in the given example).
The second part took me longer than expected because
1. I couldn't really reuse my "traversing" technique from part 1. I had to change it to conform to the clockwise movement.
2. I tripped over several stupid errors (checking col-index against height instead of width).
3. Forgetting about casting to double when dividing...d'oh. I would have noticed earlier, but it worked suprisingly well without it.


* First solution: 303
* Second solution: 408
