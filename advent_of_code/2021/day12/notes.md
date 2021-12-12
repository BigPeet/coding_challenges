To note: Both start and end are small caves, so once a path reaches the end, that's it.
I assume there is no cycle, e.g. a connection between to big caves.
I also assume connections are only listed once in the input.

One algorithm to find all such paths would be the following:

* Start with
    * an empty path,
    * an empty list of found paths,
    * and an empty queue.
* Add the start cave with the empty path (as tuple) into the queue.
* While the queue is not empty:
    * Pop the cave-path tuple from the queue. The cave becomes the current cave.
    * Add the current cave to the path.
    * Is the current cave 'end'? Add the path to the list of found paths.
    * Otherwise, take all the connections of the current cave and remove all small caves, which have already been visited in the path.
    * If there are no such connections, abandon this path.
    * Otherwise, add the connected cave with a copy of the path to the queue.
    * Repeat.

The solution for the first part is 4411.
