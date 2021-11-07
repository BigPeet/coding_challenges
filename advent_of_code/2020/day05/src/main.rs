use day05::Seat;
use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let seats: Vec<Seat> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(|lines| lines.into_iter().map(|line| line.parse::<Seat>()).collect())?;

    println!(
        "Part 1: The highest ID is {}.",
        seats.iter().map(|s| s.id()).max().unwrap_or(0)
    );

    Ok(())
}
