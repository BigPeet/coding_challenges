use day07::{Bag, Rule};
use parsing::InputError;
use std::collections::VecDeque;
use std::env;

fn main() -> Result<(), InputError> {
    let rules: Vec<Rule> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(|lines| lines.into_iter().map(|line| line.parse::<Rule>()).collect())?;

    let target = Bag::new("shiny", "gold");

    // Part 1
    let mut rules_p1: Vec<&Rule> = rules.iter().collect();
    let mut search_q = VecDeque::from([target.clone()]);
    let mut counter = 0;
    while !search_q.is_empty() {
        let cur = search_q.pop_front().unwrap();
        let (allows, remaining): (Vec<&Rule>, Vec<&Rule>) =
            rules_p1.into_iter().partition(|r| r.allows(&cur));

        for rule in allows.into_iter() {
            search_q.push_back(rule.container().clone());
            counter += 1;
        }
        rules_p1 = remaining;
    }
    println!("Part 1: {} bags can hold {}.", counter, target);

    // Part 2
    let mut search_q = VecDeque::from([(1, &target)]);
    let mut counter = 0;
    while !search_q.is_empty() {
        let cur = search_q.pop_front().unwrap();
        if let Some(rule) = rules.iter().find(|r| *r.container() == *cur.1) {
            for content in rule.contents().iter() {
                let increment = cur.0 * content.amount;
                counter += increment;
                search_q.push_back((increment, &content.bag));
            }
        }
    }
    println!("Part 2: {} bags are needed inside {}.", counter, target);

    Ok(())
}
