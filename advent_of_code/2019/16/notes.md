FFT is probably a hint towards Fast Fourier transforms. But I think they used real world acronyms before.

I might start with the "naive" approach, but I expect when trying to crack the input, this will run into timing issues.

One first observation that might help to cut down on unnecessary multiplications and/or loops:
Multiplications with 0 (roughly half of multiplications for the first ouput and almost all for the last output) can be ignored.

Ignoring for now, that the very first pattern value is ignored once:

For the 1st value, you skip 1 value (due to multiplication with 0), take 1 value , skip 1 value and take negative of value.
Repeat.

For the 2nd value, you skip 2 values, take 2, skip 2, negate 2. Repeat until no values are left.

For the n-th value: you skip n values, take n values, skip n values, negate n values. Repeat until no values are left.


Now applying the first offset:

For the n-th value: You skip (n-i) values, take n values, skip n values, negate n values. set i=0. Repeast until no values are left.

For the second part, I will need to reduce the time.
I can imagine that I have to use the fact somehow, that the last row is only the last number, the second to last row is the last number and the second to last number, etc. (applying the ones-reduction ofc).
This way the second half of the input can be transformed rather quickly.

I'm pretty sure that further optimization would require to identify which partial sums can be reused, etc.
But in the end I found that the offset points us towards the second half of the output sequence anyway.

This feels somewhat cheap, but allows me to skip further optimizations and finish this one.

* First Solution: 29956495
* Second Solution: 73556504
