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
    let messages = &groups[1];
    let rule_zero = &rules[0];
    println!(
        "Part 1: The number of valid messages is {}.",
        messages
            .iter()
            .filter(|msg| rule_zero.matches(msg, &rules))
            .count()
    );

    // Part 2
    // Replace rule 8
    if let Some(rule) = rules.iter_mut().find(|r| r.get_id() == 8) {
        *rule = Rule::Or(8, vec![42], vec![42, 8]);
    }

    // Replace rule 11
    if let Some(rule) = rules.iter_mut().find(|r| r.get_id() == 11) {
        *rule = Rule::Or(11, vec![42, 31], vec![42, 11, 31]);
    }

    let rule_zero = &rules[0];
    println!(
        "Part 2: The number of valid messages is {}.",
        messages
            .iter()
            .filter(|msg| rule_zero.matches(msg, &rules))
            .count()
    );

    Ok(())
}
