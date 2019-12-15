Reusing the intcode from Day 11.

The second part had some weird timing issues.
When HALTING the program, the remaining outputs were not received and somewhat incorrect (e.g. final score received 0).
This is why I needed to give the other thread some time and/or wake the thread up.
Also a sleep between the output and potential input was necessary to have a stable game without deadlock.

I'm not sure this will run on all machines in the same way...

First Solution: 326
Second Solution: 15988
