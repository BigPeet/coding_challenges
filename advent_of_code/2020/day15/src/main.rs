use parsing::ParsingResult;
use std::{collections::HashMap, env};

fn take_turn(last_number: &mut u32, history: &mut HashMap<u32, usize>, round: usize) {
    let new_number;
    if let Some(prev_round) = history.get(last_number) {
        let age = round - 1 - *prev_round;
        new_number = age as u32;
    } else {
        new_number = 0;
    }
    history.insert(*last_number, round - 1);
    *last_number = new_number;
}

fn main() -> ParsingResult {
    let text = parsing::filepath_from_args(env::args().collect()).and_then(parsing::read)?;

    let numbers: Vec<u32> = text
        .split(',')
        .filter_map(|tok| tok.trim().parse::<u32>().ok())
        .collect();

    // Part 1
    let mut history: HashMap<u32, usize> = HashMap::new();

    // Insert starting numbers into history, but not the last.
    for (i, num) in numbers[..numbers.len() - 1].iter().enumerate() {
        history.insert(*num, i + 1);
    }
    let mut last_number = numbers[numbers.len() - 1];

    // Play remaining rounds until the 2020th number is spoken.
    for round in (numbers.len() + 1)..=2020 {
        take_turn(&mut last_number, &mut history, round);
    }
    println!("Part 1: The 2020th number spoken is {}.", last_number);

    Ok(())
}
