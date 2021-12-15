Well instead of just finding any path, e.g. with DFS or BFS, this is a straight-forward shortest-path problem in a graph with weighted edges.
The "classic" [Dijkstra algorithm](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm) seems to be a perfect match.
The only issue might be that the input (100x100 square) has "a lot of" nodes and edges and it might become more complex for part 2.

The solution for the first part is 390.

While slow (~25-30s) the solution for the first part also works for the second part.

The solution for the second part is 2814.

Update: By replacing the visited vector, which had to be searched for the next minimum every time, with a BinaryHeap, the algorithm sped up immensly.
The second part now finished in ~25ms.
