This task builds upon the "intcode" implementation from Day 2 and Day 5.
For this reason, I've copied the Day 5 implementation as starting point for this Task.
But since the focus of this tasks is less on updating actual "intcode computer", I put the previous code into its own header and source files.

For the second part of the task, I will add a second executable since that will make it easier to support both parts.
I guess the solution for part two includes starting the intcodes in separate threads and have the threads communicate (input & output).
I will see if I can continue to use stdin and stdout for communication, but I doubt it.
Instead I will have to look into shared memory or inter-thread communication.

I run into some issues with multi-threading (as expected).
Some things I want to check:
  * Where is the deadlock happening? What is causing the deadlock?
  * What is causing the segmentation fault that is happening sometimes?
  * What happens without the mutex?
  * What happens without the "consumed" checks?
  * How to properly debug multi-threaded programs?
  * Maybe split input in phase and input to remove one conflict (this would be a solution fitted onto the amplifier problem and not a general solution :-/)
  * Maybe look at other people's solution to get a better idea.

* First solution: 336376
