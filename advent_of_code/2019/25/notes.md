This seems to be very similar to other ASCII problems, e.g. Day 21.
But this problem seems to rely more on an "interactive" approach to find the solution.

So I will output the robot's text to stdout and provide input via stdin.
Maybe AFTER having a better understanding of the map and robot's output, either a hard-coded solution (specific for my input) or "intelligent" solution (flexible for any input program) could be implemented.

"Handdrawn" Map and other info gathered from drone:

**Map of Ship**

                                                Security Checkpoint ------ Engineering
           Warp Drive Maint. - Sick Bay - Stables         |                     |
                                            |    Pressure-Sensitive Floor       |                 Storage
                                            |                                   |                    |
                           Hallway          |                           Gift Wrapping Center - Crew Quarters - Observatory - Holodeck
                              |             |                                   |                                   |
                          Navigation  - Hull Breach (Start) -------------------------------------------------- Science Lab - Passages
                              |                                                 |
                           Arcade - Corridor        Hot Chocolate Fountain - Kitchen

**Items**
* planetoid           - Sick Bay
* spool of cat6       - Warp Drive Maint.
* darkmatter          - Hallway
* photons             - Corridor
* escape pod          - Science Lab
* sand                - Observatory
* giant electromagnet - Holodeck
* coin                - Crew Quarters
* infinite loop       - Gift Wrapping Center
* molten lava         - Engineering
* wreath              - Kitchen
* fuel cell           - Hot Chocolate Fountain
* jam                 - Storage

**Notes**
* Taking the photons will get you eaten!
* Taking the escape pod will launch you into space!
* Taking the giant electromagnet will get it stuck to you (at least if you only have it)!
* Taking the infinite loop will get in fact trigger an infinite loop.
* Taking the molten lava will melt you.
* The Pressure-Sensitive Floor behind the Security Checkpoint wants the drone to be slightly heavier than empty.
    * All (7) harmless items is too much weight.
    * The coin alone is too much, but everything BUT the coin is too light...something seems off.
    * Either I'm missing an item or the combinations don't make any sense and I just have to find one that fits.
    * Update: I finally found the storage, which contains the jam! For some reason I kept missing that door.

So for part 1 I implemented a "hybrid" solution:
* You can either play fully interactively.
* Or you can provide a list of commands.
    * The commands either include ALL actions you need to finish (i.e. taking the correct items you need for the pressure sensitive floor).
    * OR the commands should lead you to the Security Checkpoint with all 8 harmless items and then it will bruteforce the item combinations.
        * This is indicated by another commandline parameter.
        * The last command in the command list then should be the one required to pass into the room.

Solution: 8401920

The next step would be to implement a general solution, e.g. using a BFS to gather all the items and discover the path to the Pressure-Sensitive Floor.
But I will likely not implement this and move on to the 2020 challenges instead.
