use parsing::ParsingResult;

fn count_increasing(numbers: &[i32]) -> usize {
    let mut count = 0;
    for (i, val) in numbers.iter().skip(1).enumerate() {
        if *val > numbers[i] {
            count += 1;
        }
    }
    count
}

fn count_windows(numbers: &[i32], window_size: usize) -> usize {
    let mut count = 0;
    for (i, val) in numbers.iter().skip(window_size).enumerate() {
        if *val > numbers[i] {
            count += 1;
        }
    }
    count
}

fn main() -> ParsingResult {
    let numbers: Vec<i32> = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    println!(
        "Part 1: The number of increasing numbers is {}.",
        count_increasing(&numbers)
    );

    // Part 2
    println!(
        "Part 2: The number of increasing sums is {}.",
        count_windows(&numbers, 3)
    );

    Ok(())
}
