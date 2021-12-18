use day18::SnailNumber;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let numbers: Vec<SnailNumber> = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    let mut sum = SnailNumber::empty();
    for n in numbers.clone() {
        sum = sum + n;
    }
    println!(
        "Part 1: The magnitude of the final sum is {}.",
        sum.magnitude()
    );

    // Part 2
    let mut largest_mag = u32::MIN;
    for (i, n1) in numbers.iter().enumerate() {
        for (j, n2) in numbers.iter().enumerate() {
            if i != j {
                let sum = n1.clone() + n2.clone();
                let mag = sum.magnitude();
                if mag > largest_mag {
                    largest_mag = mag;
                }
            }
        }
    }
    println!(
        "Part 2: The largest magnitude of just two different numbers is {}.",
        largest_mag
    );

    Ok(())
}
