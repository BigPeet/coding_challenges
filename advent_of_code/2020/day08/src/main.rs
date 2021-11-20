use day08::{Instruction, Program};
use parsing::InputError;
use std::collections::HashSet;
use std::env;

fn main() -> Result<(), InputError> {
    let instructions: Vec<Instruction> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(|lines| {
            lines
                .into_iter()
                .map(|line| line.parse::<Instruction>())
                .collect()
        })?;

    let mut prog = Program::new(instructions);

    // Part 1
    let mut set = HashSet::new();
    set.insert(0); // start value
    while set.insert(prog.step()) {}
    println!(
        "Part 1: Accumulator value before executing first instruction twice: {}",
        prog.acc_value()
    );

    Ok(())
}
