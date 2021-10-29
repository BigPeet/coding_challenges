The first part looks relatively easy to implement.

The naive first approach would be:

* Create 50 ASCII instances from the given input and start them.
* Have a single controller:
    * has 50 packet queues, one for each instance
    * first loop over all ASCII instances and gives them an address.
    * then continuously loop over the instances and 
        * check if they have output (a packet) => store packet in queue for receiver
        * if they dont have output, check if they request input and if something is in their queue => send it
    * Continue until someone sends a packet to address 255

**Considerations**

* The current intcode implementation would require to start a new thread for each instance. It might be problematic to run 50 different threads. An alternative would be to add an alternative to the "execute()" function which performs steps until an IO operation is hit. Then the controller could use that function to let the instances work sequentially, not in parallel.
* Another issue might be the memory requirement. I forgot how much memory every ASCII instances requires (it's been a while since I've looked at the code), so I'm not sure if 50 instances would cause issues. And there might be more in part two.
