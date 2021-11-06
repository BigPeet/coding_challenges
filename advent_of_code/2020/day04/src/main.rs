use day04::{PassportP1, PassportP2};
use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let chunks: Vec<String> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::read)?
        .split("\n\n")
        .map(str::to_string)
        .collect();

    let mut passports_p1: Vec<PassportP1> = Vec::with_capacity(chunks.len());
    let mut passports_p2: Vec<PassportP2> = Vec::with_capacity(chunks.len());
    for chunk in chunks.iter() {
        passports_p1.push(chunk.parse()?);
        passports_p2.push(chunk.parse()?);
    }

    println!(
        "Part 1: There are {} valid passports.",
        passports_p1.iter().filter(|pp| pp.is_valid()).count()
    );
    println!(
        "Part 2: There are {} valid passports.",
        passports_p2.iter().filter(|pp| pp.is_valid()).count()
    );

    Ok(())
}
