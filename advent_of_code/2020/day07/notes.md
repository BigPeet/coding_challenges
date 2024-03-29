So this one is more interesting than the last ones.

First the rules have to be parsed into "containing bag" and "contained bags".
Each bag has a unique identifier, an adjective + a color, (and as far as I can tell) every rule has a unique containing bag.
The amount of contained bags is not important for part 1 but might become important for part 2.

After parsing, check which bags can contain the shiny gold bag directly.
Add those to the "search queue" and remove their rules (to avoid duplicate counts, cycles, etc.).
Then search for bags which can contain those in the queue directly, and so on.

The solution for the first part is 197.

Part two is similar:
Search the list of rules for the rule about the current "containing" bag.
Count the "contained" bags and add those bags to the search queue together with their multiplier, e.g. if the "parent" bag is contained 2 times and contains 11 of this currently search bag, then the amounts need to be multiplied by 22.
There are definitely some chances for optimization here:
* Other collections could be used, e.g. because the naive approach searches a lot, a collection with constant search time could improve the execution time.
* Intermediate results could be stored and reused. E.g. if the programm calculated that one bag A contains X other bags, then X does not need to be calculated again, if bag A is encountered again.

The solution for the second part is 85324.
