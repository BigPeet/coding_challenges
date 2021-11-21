use day09::find_invalid_number;
use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let cmd_args: Vec<String> = env::args().collect();
    let preamble_length = match cmd_args
        .last()
        .and_then(|value| value.parse::<usize>().ok())
    {
        Some(value) => value,
        None => 25,
    };
    let numbers = parsing::filepath_from_args(cmd_args)
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    if let Some(invalid) = find_invalid_number(numbers.as_slice(), preamble_length) {
        println!("Part 1: The invalid number is {}.", invalid);
    } else {
        println!("No invalid number found!");
    }

    Ok(())
}
