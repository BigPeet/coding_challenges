use day17::Area;
use parsing::{InputError, ParsingResult};

fn main() -> ParsingResult {
    let area = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::read)?
        .parse::<Area>()?;

    // Part 1
    let viable_velocities = day17::viable_velocities(&area);
    let max_height =
        day17::find_highest_position(&viable_velocities).ok_or(InputError::ParseGeneral)?;
    println!("Part 1: The highest y position is {}.", max_height);
    println!(
        "Part 2: There are {} distinct viable velocity values.",
        viable_velocities.len()
    );

    Ok(())
}
