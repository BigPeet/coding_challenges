use day18::Expression;
use parsing::ParsingResult;
use std::env;

fn main() -> ParsingResult {
    let expressions: Vec<Expression> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    println!(
        "Part 1: The sum of all resulting values is {}.",
        expressions.iter().map(|exp| exp.evaluate()).sum::<u64>()
    );

    Ok(())
}
