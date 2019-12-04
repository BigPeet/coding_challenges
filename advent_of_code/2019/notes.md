# General notes


## 2019-12-04 

After completing the Tasks of the first 3 days, I noticed that I spent my time for these tasks somewhat like this:

* 15% on thinking about and implementing data structures and algorithms to solve the concrete task
* 75% on reading and parsing the input file into the data structures
* 5% on creating tests
* 5% on adding input and boundary checks

To focus more on the actual solution to the task, I will try to reduce the "overhead" (i.e. input checks and reading/parsing) for me a little bit.
Most of the time I spent in reading and parsing is spent on trying to handle dynamically sized input, allocating and freeing the appropriate amount of memory, etc.
I will try to read up on how to do this kind of parsing more elegantly.
But depending on the task, I will allow myself to have more assumption about the input (e.g. for the Day 3 task I could have just assumed there will always be 2 lines/wires per file).
I might even move reading/parsing the input outside of C.
I might switch to either using Bash or Python to parse the input into command line arguments for the C executeable.

Additionally, I will only do input checks for example in "public" functions, not static ones.
I assume a static function is only called with already checked input.
