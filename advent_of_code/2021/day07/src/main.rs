use parsing::ParsingResult;
use std::num::ParseIntError;

fn main() -> ParsingResult {
    let mut numbers = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::read)?
        .trim()
        .split(',')
        .map(|tok| tok.parse::<u32>())
        .collect::<Result<Vec<u32>, ParseIntError>>()?;

    let median = day07::median(&mut numbers);
    let total_fuel = day07::fuel_cost(&numbers, median);
    println!(
        "Part 1: The horizontal position {} consumes {} fuel.",
        median, total_fuel
    );

    Ok(())
}
