use day14::{FastPolymerisation, InsertionRule, Polymer, Polymerisation};
use parsing::{InputError, ParsingResult};

fn main() -> ParsingResult {
    let mut groups = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::get_line_groups)?;

    let polymer = Polymer::try_from(groups[0].remove(0))?;
    let rules = groups[1]
        .iter()
        .map(|l| l.parse::<InsertionRule>())
        .collect::<Result<Vec<InsertionRule>, InputError>>()?;

    // Part 1
    let steps = 10;
    let mut polymerisation = Polymerisation::new(polymer.clone(), rules.clone());
    (0..steps).for_each(|_| polymerisation.step());
    let (min, max) = polymerisation.min_max();
    println!("Part 1: After {} steps, the difference between the quantity of the most common element and least common is {}.", steps, max - min);

    // Part 2
    let steps = 40;
    let mut fast_poly = FastPolymerisation::new(polymer, rules);
    (0..steps).for_each(|_| fast_poly.step());
    let (min, max) = fast_poly.min_max();
    println!("Part 2: After {} steps, the difference between the quantity of the most common element and least common is {}.", steps, max - min);

    Ok(())
}
