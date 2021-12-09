use day09::HeightMap;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let lines =
        parsing::filepath_from_args(std::env::args().collect()).and_then(parsing::get_lines)?;

    let hmap = HeightMap::new(lines)?;

    // Part 1
    let low_points = hmap.low_points();
    let risk_levels = day09::risk_levels(low_points);
    let mut sum = 0u32;
    risk_levels.iter().for_each(|rl| sum += *rl as u32);
    println!(
        "Part 1: The sum of the risk levels of all low points is {}.",
        sum
    );

    Ok(())
}
