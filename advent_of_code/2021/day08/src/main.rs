use day08::{Entry, SSDisplay};
use parsing::{InputError, ParsingResult};

fn main() -> ParsingResult {
    let entries: Vec<Entry> = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    let mut easy_digits = 0;
    entries
        .iter()
        .for_each(|e| easy_digits += e.outputs().iter().filter(|d| d.is_easy()).count());
    println!(
        "Part 1: 'Easy' digits appear {} times in the output values.",
        easy_digits
    );

    // Part 2
    let mut sum = 0;
    for entry in entries.iter() {
        let wiring = entry.deduce_wiring().ok_or(InputError::ParseGeneral)?;
        sum += SSDisplay::flawed(wiring)
            .value(entry.outputs())
            .ok_or(InputError::ParseGeneral)?;
    }
    println!("Part 2: The sum of all output values is {}.", sum);

    Ok(())
}
