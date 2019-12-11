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

Update: I just realized that the deadlocks are caused because I misinterpreted the Task. I thought the phase setting had to applied every cycle.
So I assumed every instance of the program would need two inputs and then output its result in every cycle.
After (a lot of) debugging (gdb and logging) I noticed that the program would output its result after receiving the phase for the 2nd time.
I looked at the op-codes and the program itself (test\_input\_04.txt) and noticed my mistake: The first "input" for storing the phase is only executed in the first pass.

After fixing that, I could resolve the remaining deadlocks :-)
I could have tried to solve the 2nd part without multi-threading, but I (re-)learned a lot from this Task...how to use GDB to debug multiple threads, how to use valgrind, how to properly use pthread.

* First solution: 336376
* Second solution: 21596786
