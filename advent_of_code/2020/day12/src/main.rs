use day12::{Ferry, Instruction};
use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let instructions: Vec<Instruction> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    let mut ferry = Ferry::new();
    for inst in instructions.iter() {
        ferry.apply(inst);
    }
    println!(
        "Part 1: The manhattan distance of the ferry is {}.",
        ferry.position().manhattan_distance()
    );

    // Part 2
    let mut ferry = Ferry::with_waypoint();
    for inst in instructions.iter() {
        ferry.apply(inst);
    }
    println!(
        "Part 2: The manhattan distance of the ferry with waypoint is {}.",
        ferry.position().manhattan_distance()
    );

    Ok(())
}
