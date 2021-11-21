I'm starting late on this challenge and will likely not finish it before the 2021 advent starts.
I might even interrupt this one to finish the 2021's one first.

While I implemented 2019's advent mostly in C, I will use 2020's advent to get some first experience with Rust.
I will start by creating a workspace, adding a separate package per "day".
If this does not really suit the challenges, I might change the setup.
E.g. to using a single package and only creating a binary crate for each "day".

While solving the tasks I will try to learn and understand how to do certain things in Rust.
If I'm not satisfied with some piece of code, but it works at least, I might add a FIXME comment to revisit this later.

In general, to build and run the solution for dayXY use the following commands:

```bash
cargo build -p dayXY
cargo run --bin dayXY dayXY/input.txt [OPTIONS]
```
