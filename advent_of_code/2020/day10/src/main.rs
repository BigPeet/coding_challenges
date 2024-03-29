use day10::{calculate_combinations, separation_points};
use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let mut numbers: Vec<usize> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Add charing outlet joltage (0) and device joltage (max + 3).
    numbers.push(0);
    numbers.push(numbers.iter().max().unwrap() + 3);
    numbers.sort_unstable();

    // Part 1
    let mut diff_counts: [usize; 3] = [0; 3];
    for i in 0..numbers.len() - 1 {
        let diff = numbers[i + 1] - numbers[i];
        if diff <= 3 {
            diff_counts[diff - 1] += 1;
        } else {
            println!("There is a difference larger than 3!");
            return Err(InputError::ParseGeneral);
        }
    }
    println!(
        "Part 1: There are {} differences of 1 and {} differences of 3. Product: {}",
        diff_counts[0],
        diff_counts[2],
        diff_counts[0] * diff_counts[2]
    );

    // Part 2
    let mut total_combinations = 1;
    let separators = separation_points(numbers.as_slice());
    let mut prev_idx = 0;
    for sep_idx in separators.iter() {
        let range = &numbers[prev_idx..*sep_idx];
        let combinations = calculate_combinations(range);
        total_combinations *= combinations;
        prev_idx = *sep_idx;
    }
    println!(
        "Part 2: There are {} total combinations possible.",
        total_combinations
    );

    Ok(())
}
