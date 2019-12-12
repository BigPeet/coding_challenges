# Assert

When to use assert() and when to use other condition checks (e.g. if)?

assert() should be used during development to basically check the assumption you as the developer make about your code.
It should check things that "can't happen" and warn you if those assumptions are false.
If an assert triggers, it should be a bug, not wrong input.
Assert does not replace input validation.
