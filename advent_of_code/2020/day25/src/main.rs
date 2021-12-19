use day25::ValueType;
use parsing::{InputError, ParsingResult};

fn main() -> ParsingResult {
    let keys: Vec<ValueType> = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    if keys.len() != 2 {
        return Err(InputError::ParseGeneral);
    }

    // Part 1
    let first_loop_size = day25::calculate_loop_size(keys[0]);
    println!(
        "Part 1: The encryption key is {}.",
        day25::transform(keys[1], first_loop_size)
    );
    Ok(())
}
