use day18::SnailNumber;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let numbers: Vec<SnailNumber> = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    let mut sum = SnailNumber::empty();
    for n in numbers {
        sum = sum + n;
    }
    println!(
        "Part 1: The magnitude of the final sum is {}",
        sum.magnitude()
    );

    Ok(())
}
