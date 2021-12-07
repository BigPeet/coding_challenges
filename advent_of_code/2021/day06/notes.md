The solution for the first part is 386755.

The second part can not easily solved by just continuing the simulation naively.
The population grows too large to update every single fish.

But the simulation can be simplified, by binning the timings of the fishes together and applying 7-day-steps.
After 7 days ...
* All fishes which were in state 0-6, will be in the exact same state and will have spawned one additional fish each which is now in the parent's state + 2.
* All fishes which were in state 7 or 8, will now be in state 0 or 1 and won't have spawned another fish.
So a faster simulation could simulate the first N days (with N = TOTALDAYS % 7), pack the fishes into these "bins" and the simply update the bins M times (M = TOTALDAYS / 7) like this:
    * For i in 0..=6 => new\_bin\_value[i] = old\_bin\_value[i] + old\_bin\_value[i - 2 % (7+2)]
    * For i in 7..=8 => new\_bin\_value[i] = old\_bin\_value[i - 2]
In the end just sum all the bin values.

The solution for the second part is 1732731810807.
