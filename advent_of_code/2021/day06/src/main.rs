use day06::FishSchool;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let mut fish_school = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::read)?
        .parse::<FishSchool>()?;

    // Part 1
    let days = 80;
    fish_school.simulate(days);
    println!(
        "Part 1: After 80 days, there would be {} lantern fish.",
        fish_school.len()
    );

    Ok(())
}
