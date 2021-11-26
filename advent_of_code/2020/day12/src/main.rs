use day12::{Ferry, Instruction, Navigable, Position, WaypointFerry};
use parsing::InputError;
use std::env;

fn find_target<T: Navigable>(mut ship: T, instructions: &[Instruction]) -> Position {
    for inst in instructions.iter() {
        ship.apply(inst);
    }
    ship.position()
}

fn main() -> Result<(), InputError> {
    let instructions: Vec<Instruction> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    println!(
        "Part 1: The manhattan distance of the ferry is {}.",
        find_target(Ferry::new(), &instructions).manhattan_distance()
    );

    // Part 2
    println!(
        "Part 2: The manhattan distance of the ferry with waypoint is {}.",
        find_target(WaypointFerry::new(), &instructions).manhattan_distance()
    );

    Ok(())
}
