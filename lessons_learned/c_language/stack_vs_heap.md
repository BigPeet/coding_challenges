# Stack vs Heap

There is a lot to be said about allocating variables on the stack or the heap.
Here is nice comparison I found [here](https://www.geeksforgeeks.org/stack-vs-heap-memory-allocation/).

| Parameter | STACK | HEAP |
|---------:|------:|-----:|
| Basic | Memory is allocated in a contiguous block. | Memory is allocated in any random order. |
| Allocation and Deallocation | Automatic by compiler instructions. | Manual by programmer. |
| Implementation | Hard | Easy |
| Cost | Less | More |
| Access time | Faster | Slower |
| Main Issue | Shortage of memory | Memory fragmentation |
| Locality of reference | Excellent | Adequate |
| Flexibility | Fixed size | Resizing is possible |


What I gathered from various sources:

* Use the stack, when you do not need dynamic size, the struct is not too large (to prevent stack overflow) and/or the memory does not need to persist outside of the current scope.
* valgrind allows you to easily check for memory leaks due to missed free() calls.
