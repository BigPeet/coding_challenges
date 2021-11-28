use day18::{AdvancedParser, Expression, ExpressionParser, NormalParser};
use parsing::{InputError, ParsingResult};
use std::env;

fn main() -> ParsingResult {
    let lines = parsing::filepath_from_args(env::args().collect()).and_then(parsing::get_lines)?;

    // Part 1
    let expressions = lines
        .iter()
        .map(|l| NormalParser::parse_line(l))
        .collect::<Result<Vec<Expression>, InputError>>()?;

    println!(
        "Part 1: The sum of all resulting values is {}.",
        expressions.iter().map(|exp| exp.evaluate()).sum::<u64>()
    );

    // Part 2
    let expressions = lines
        .iter()
        .map(|l| AdvancedParser::parse_line(l))
        .collect::<Result<Vec<Expression>, InputError>>()?;

    println!(
        "Part 2: The sum of all resulting values is {}.",
        expressions.iter().map(|exp| exp.evaluate()).sum::<u64>()
    );

    Ok(())
}
