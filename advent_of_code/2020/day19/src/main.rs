use day19::Rule;
use parsing::{InputError, ParsingResult};
use std::env;

fn main() -> ParsingResult {
    let groups = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::get_line_groups)?;

    let mut rules = groups[0]
        .iter()
        .map(|l| l.parse::<Rule>())
        .collect::<Result<Vec<Rule>, InputError>>()?;
    rules.sort_unstable();

    // Part 1
    let valid = rules[0].rule.evaluate(&rules);
    let messages = &groups[1];
    println!(
        "Part 1: The number of valid messages is {}.",
        messages.iter().filter(|msg| valid.contains(*msg)).count()
    );

    Ok(())
}
