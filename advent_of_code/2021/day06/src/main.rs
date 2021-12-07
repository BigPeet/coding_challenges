use day06::FishSchool;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let fish_school = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::read)?
        .parse::<FishSchool>()?;

    // Part 1
    let days = 80;
    let mut p1_school = fish_school.clone();
    p1_school.simulate(days);
    println!(
        "Part 1: After {} days, there would be {} lantern fish.",
        days,
        p1_school.len()
    );

    // Part 2
    let days = 256;
    println!(
        "Part 2: After {} days, there would be {} lantern fish.",
        days,
        fish_school.fast_simulation(days)
    );

    Ok(())
}
