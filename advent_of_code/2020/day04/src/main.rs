use day04::Passport;
use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let chunks: Vec<String> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::read)?
        .split("\n\n")
        .map(str::to_string)
        .collect();

    let mut passports: Vec<Passport> = Vec::with_capacity(chunks.len());
    for chunk in chunks.iter() {
        passports.push(chunk.parse()?);
    }
    println!(
        "Valid passports: {}",
        passports.iter().filter(|pp| pp.is_valid()).count()
    );

    Ok(())
}
