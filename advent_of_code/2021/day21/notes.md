Part 1 is clearly just the setup task for this day.
The solution for the first part is 903630.

Part 2 required some changes to the API and some trick to avoid simulating all the possible splits.

Theoretically, every roll splits the game into `3^3 = 27` new universes.
But for any given game state `S` a player's turn can lead to only 7 distinct follow-up states `S1..S7`, since the possible values range from 3 to 9.
While `S1` and `S7` are only reached in one of the new universes (1-1-1 and 3-3-3, respectively), `S4` is reached in 7 of these new splits.

So we can track a game state `S` and in how many splits `P` it appears.
We can then look at all the rolls for the active player and get `S1..S7` and multiply `P` with their corresponding split factor (resulting in `P1..P7`).
If `Si` is terminal state, we add `Pi` winning splits to the active player's total.
For all non-terminal states, `(Si, Pi)` is stored to explore further in the next player's turn.
In addition, if a state is reached through different rolls from different predecessor states, e.g. player 1 reaching position 9 by starting from 6 and rolling a 3 or by starting from 5 and rolling a 4, then the reached state accumulates their splits.

Start with the initial game state, i.e. the starting positions, and a split value of 1.
Then play with players changing every turn and explore all tracked game states and their splits until all states are terminal.

The solution for the second part is 303121579983974.
