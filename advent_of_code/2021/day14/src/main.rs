use day14::{FastPolymerisation, InsertionRule, Polymer, Polymerisation};
use parsing::{InputError, ParsingResult};

fn main() -> ParsingResult {
    let mut groups = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::get_line_groups)?;

    // Part 1
    let polymer = Polymer::try_from(groups[0].remove(0))?;
    let rules = groups[1]
        .iter()
        .map(|l| l.parse::<InsertionRule>())
        .collect::<Result<Vec<InsertionRule>, InputError>>()?;

    let mut polymerisation = Polymerisation::new(polymer.clone(), rules.clone());
    let steps = 10;
    for _ in 0..steps {
        polymerisation.step();
    }
    let occurrences = polymerisation.polymer().occurrences();
    let max = occurrences.iter().map(|(_, occ)| occ).max().unwrap_or(&0);
    let min = occurrences.iter().map(|(_, occ)| occ).min().unwrap_or(&0);
    println!("Part 1: After {} steps, the difference between the quantity of the most common element and least common is {}.", steps, max - min);

    // Part 2
    let mut fast_poly = FastPolymerisation::new(polymer, rules);
    let steps = 40;
    for _ in 0..steps {
        fast_poly.step();
    }
    let occurrences = fast_poly.occurrences();
    let max = occurrences.iter().map(|(_, occ)| occ).max().unwrap_or(&0);
    let min = occurrences.iter().map(|(_, occ)| occ).min().unwrap_or(&0);
    println!("Part 2: After {} steps, the difference between the quantity of the most common element and least common is {}.", steps, max - min);

    Ok(())
}
