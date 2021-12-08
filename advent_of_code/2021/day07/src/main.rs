use parsing::ParsingResult;
use std::num::ParseIntError;

fn main() -> ParsingResult {
    let mut numbers = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::read)?
        .trim()
        .split(',')
        .map(|tok| tok.parse::<u32>())
        .collect::<Result<Vec<u32>, ParseIntError>>()?;

    // Part 1
    let median = day07::median(&mut numbers);
    let total_fuel = day07::fuel_cost(&numbers, median, day07::abs_diff);
    println!(
        "Part 1: The horizontal position {} consumes {} fuel.",
        median, total_fuel
    );

    // Part 2
    let mean = day07::mean(&numbers);
    let mean_ceil = mean.ceil() as u32;
    let mean_floor = mean.floor() as u32;
    let total_fuel_ceil = day07::fuel_cost(&numbers, mean_ceil, day07::crab_diff);
    let total_fuel_floor = day07::fuel_cost(&numbers, mean_floor, day07::crab_diff);
    let pos;
    let total_fuel;
    if total_fuel_ceil < total_fuel_floor {
        pos = mean_ceil;
        total_fuel = total_fuel_ceil;
    } else {
        pos = mean_floor;
        total_fuel = total_fuel_floor;
    }

    println!(
        "Part 2: The horizontal position {} consumes {} fuel.",
        pos, total_fuel
    );

    Ok(())
}
