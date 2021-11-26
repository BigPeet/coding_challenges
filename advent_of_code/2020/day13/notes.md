To get the wait time for each line, you simply have to take the modulo and substract it from the line (if the modulo is not 0):

```
mod = (leave_time % line_no)
wait_time = (mod == 0) ? 0 : line_no - (leave_time % line_no)
```

The solution for the first part is 3997.

The second part is definitely more tricky.
You could check every possible wait time by constructing a system of congruences like this (for the initial example):

```
for t in 0.. {
    let valid = t % 7 == 0 && (t + 1) % 13 == 0 %% (t + 4) % 59 == 0 ...
    // OR
    let valid = t % 7 == 0 && t % 13 == 12 && t % 59 == 55 ...
}
```

But that would require checking a lot of timestamps until the correct one is found.
Therefore, I think the solution must be calculable.

A google search brings up the [Chinese remainder theorem](https://en.wikipedia.org/wiki/Chinese_remainder_theorem), which could be used to find the solution to this system of congruences.
All the bus lines (and therefore modulo divisors) are prime.
What could help here is either the "direct construction" (a nice writeup how to apply it can be found [here](https://brilliant.org/wiki/chinese-remainder-theorem/)) or searching by sieve.
The former would require an implementation of the modular multiplactive inverse, e.g. by using the extended euclidean algorithm or Fermat's little theorem ([see here](https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm#Computing_multiplicative_inverses_in_modular_structures) or [here](https://www.geeksforgeeks.org/multiplicative-inverse-under-modulo-m/)).

For the latter the search would starting with a "small" step size which increases by multiplying the different "divisors" ([see here](https://en.wikipedia.org/wiki/Chinese_remainder_theorem#Search_by_sieving)).
This might increase the step size enough to be feasible.

The solution for the second part is 500033211739354.
