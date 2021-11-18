use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let groups = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::get_line_groups)?;

    let sum = groups
        .iter()
        .fold(0, |acc, group| acc + day06::count_any(group));
    println!("Part 1: The 'any' sum of all groups is {}.", sum);

    let sum = groups
        .iter()
        .fold(0, |acc, group| acc + day06::count_all(group));
    println!("Part 2: The 'all' sum of all groups is {}.", sum);
    Ok(())
}
