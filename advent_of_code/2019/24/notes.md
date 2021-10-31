This is basically a cellular automaton (with a variation of conway's game of life).

It looks quite straight-forward to implement with the biggest consideration currently towards storing the history of "scans".
I could either assume a maximum number of scans before a repitition is found and use a fixed-size array or create a dynamically growing container (list or vector).

Two things about the biodiversity:

1. The same biodiversity for two scans should mean both scans are identical => The biodiversity can be used to easy serizalize / de-serialize the scan. That might be easier to store.
2. If a scan is encoded as little-endian binary number (bugs = 1, empty = 0), the result should be the biodiversity.
