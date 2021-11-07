use day05::Seat;
use parsing::InputError;
use std::env;

const NUM_OF_SEATS: usize = 128 * 8;

fn main() -> Result<(), InputError> {
    let seats: Vec<Seat> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(|lines| lines.into_iter().map(|line| line.parse::<Seat>()).collect())?;

    println!(
        "Part 1: The highest ID is {}.",
        seats.iter().map(Seat::id).max().unwrap_or(0)
    );

    // Allows for contant-time check if a seat is in the parsed list.
    let mut taken = [false; NUM_OF_SEATS];
    seats.iter().for_each(|s| taken[s.id()] = true);

    // Skip first and last id, can't be it.
    for id in 1..(NUM_OF_SEATS - 1) {
        if !taken[id] && taken[id - 1] && taken[id + 1] {
            println!("Part 2: Your seath ID is {}", id);
            return Ok(());
        }
    }

    Ok(())
}
