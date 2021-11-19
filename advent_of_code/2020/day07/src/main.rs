use day07::{Bag, Rule};
use parsing::InputError;
use std::collections::VecDeque;
use std::env;

fn main() -> Result<(), InputError> {
    let mut rules: Vec<Rule> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(|lines| lines.into_iter().map(|line| line.parse::<Rule>()).collect())?;

    let target = Bag::new("shiny", "gold");

    let mut search_q = VecDeque::from([target]);
    let mut counter = 0;

    while !search_q.is_empty() {
        let cur = search_q.pop_front().unwrap();
        let (allows, remaining): (Vec<Rule>, Vec<Rule>) =
            rules.into_iter().partition(|r| r.allows(&cur));

        for rule in allows.into_iter() {
            search_q.push_back(rule.container());
            counter += 1;
        }
        rules = remaining;
    }

    println!("Part 1: {} bags can hold the target.", counter);

    Ok(())
}
