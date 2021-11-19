So this one is more interesting than the last ones.

First the rules have to be parsed into "containing bag" and "contained bags".
Each bag has a unique identifier, an adjective + a color, (and as far as I can tell) every rule has a unique containing bag.
The amount of contained bags is not important for part 1 but might become important for part 2.

After parsing, check which bags can contain the shiny gold bag directly.
Add those to the "search queue" and remove their rules (to avoid duplicate counts, cycles, etc.).
Then search for bags which can contain those in the queue directly, and so on.

The solution for the first part is 197.
