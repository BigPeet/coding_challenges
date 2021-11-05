use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let mut numbers = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_numbers)?;

    numbers.sort_unstable();
    let numbers = numbers;

    println!("Part 1:");
    if let Some((a, b)) = day01::find_summands_p1(&numbers, 2020) {
        println!("Found entries: {} + {} = {}", a, b, 2020);
        println!("Multiplication: {} * {} = {}", a, b, a * b);
    } else {
        println!("No match found!");
    }
    println!("Part 2:");
    if let Some((a, b, c)) = day01::find_summands_p2(&numbers, 2020) {
        println!("Found entries: {} + {} + {} = {}", a, b, c, 2020);
        println!("Multiplication: {} * {} * {} = {}", a, b, c, a * b * c);
    } else {
        println!("No match found!");
    }

    Ok(())
}
