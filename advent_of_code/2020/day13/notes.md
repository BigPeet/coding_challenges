To get the wait time for each line, you simply have to take the modulo and substract it from the line (if the modulo is not 0):

```
mod = (leave_time % line_no)
wait_time = (mod == 0) ? 0 : line_no - (leave_time % line_no)
```

The solution for the first part is 3997.
