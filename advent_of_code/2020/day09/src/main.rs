use day09::{find_invalid_number, find_sum_range};
use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let cmd_args: Vec<String> = env::args().collect();
    let preamble_length = cmd_args
        .last()
        .and_then(|value| value.parse::<usize>().ok())
        .unwrap_or(25);
    let numbers = parsing::filepath_from_args(cmd_args)
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    if let Some(invalid) = find_invalid_number(numbers.as_slice(), preamble_length) {
        println!("Part 1: The invalid number is {}.", invalid);

        // Part 2:
        if let Some((low, high)) = find_sum_range(numbers.as_slice(), invalid) {
            let range = &numbers[low..=high];
            let max = range.iter().max().unwrap();
            let min = range.iter().min().unwrap();
            println!(
                "Part 2: Range {:?}. Min = {}, Max = {}, Sum = {}",
                range,
                min,
                max,
                min + max
            );
        } else {
            println!("No range found!");
        }
    } else {
        println!("No invalid number found!");
    }

    Ok(())
}
