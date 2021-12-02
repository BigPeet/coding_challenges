use day02::{AdvancedPosition, Command, Position};
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let commands: Vec<Command> = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    let mut pos = Position::new();
    for cmd in commands.iter() {
        pos.apply(cmd);
    }
    println!(
        "Part 1: The position of the submarine after all commands is {}. Product: {}.",
        pos,
        pos.horizontal * pos.depth
    );

    // Part 2
    let mut pos = AdvancedPosition::new();
    for cmd in commands.iter() {
        pos.apply(cmd);
    }
    println!(
        "Part 2: The position of the submarine after all commands is {}. Product: {}.",
        pos,
        pos.horizontal * pos.depth
    );

    Ok(())
}
