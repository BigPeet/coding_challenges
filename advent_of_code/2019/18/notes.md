The first step is of course parsing the input.
After that is done, the location of every key and door is known (as well as their amounts).

Then a function to "pick up" keys and "open" doors has to be implemented.

The next step is to implement a distance function (possibly reusing the one from Day 15) that gives you the distance (and maybe path) between two positions.
Start with rebuilding the distance map every time, but it could also be updated when stepping along a path.
The results could also be "cached/stored" so repeated calls from the same "from" node don't need to be recalculated.

It also has to determined, if a target position is reachable.
Using the approach of Day 15, that would be "baked" in to the distance map: Unreachable fields have a negative distance.
This might be sufficient, but for a good solution it might also be necessary to know why a field is unreachable (i.e. which doors are blocking).
But for now I will settle with simple unreachable.

After that the real challenge of part 1 begins: Finding the minimal steps to reach all keys.

My first approach will be:
* Use a recursive function giving it the map, the current position, the current count and list of keys and doors picked-up/opened, with their size
    * Not providing the key/door information separately would require to create copy of the map for every recursive call.
* If there is no reachable key: return the given count
* While there is only one reachable key:
    * go there
    * update position and add Key/door to list.
    * Count Steps
* If there are multiple options:
    * Simulate each option. Choose the one resulting in fewer steps.
    * Add that amount of steps to the count and return that.

I'm pretty sure that example works (as in will result in the correct outcome).
But it might run into time or memory issues, when used on the large input.

Okay without storing results while finding the minimal path, the algorithm runs into trouble when tackling higher key amounts.
My first idea to store "solutions" in form of key-pick order is wrong.

The important part are the remaining keys.
If there is only 1 key remaining, the solution is 0 + the distance between our current position (previously collected key) and the remaining key. So 0 could be stored for all 1-key fields.

If there are 2 keys remaining I only need to know the minimal path to solve them + the key I need to collect next. Then I know that I need the distance to that key + the stored solution for this 2-key-field.

So a solution should store the set of remaining key and the position of the first key to get.

-------------------------------

So after testing this, this will still lead to an insanely long runtime for "larger" number of keys.

So I will need to completely change my approach.
After admittedly reading up on the issue, I will try a graph-based approach.

Transforming the maze and the keys into a graph/tree allows the use of breadth-first-search to find the shortest path.

But since I've spend a lot of time on this already, I will implemenent this new approach in Python (having easy access to all the necessary data structs there).
If I have an approach that can handle the provided inputs, I will either leave the Python solution or implement it in C later.
