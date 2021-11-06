use day02::NewPolicy;
use day02::OldPolicy;
use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let lines = parsing::filepath_from_args(env::args().collect()).and_then(parsing::get_lines)?;
    println!(
        "Part 1: Number of valid passwords: {}",
        day02::count_valid_passwords::<OldPolicy>(&lines)
    );
    println!(
        "Part 2: Number of valid passwords: {}",
        day02::count_valid_passwords::<NewPolicy>(&lines)
    );

    Ok(())
}
