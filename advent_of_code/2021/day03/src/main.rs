use day03::BitCriteria;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let lines =
        parsing::filepath_from_args(std::env::args().collect()).and_then(parsing::get_lines)?;

    // Part 1
    let (gamma, epsilon) = day03::count_bits(&lines)?;
    println!(
        "Part 1: Gamma rate {}, Epsilon rate {}. Power consumption: {}.",
        gamma,
        epsilon,
        gamma * epsilon
    );

    let oxygen_gen = day03::find_rating(&lines, BitCriteria::MostCommon)?;
    let co2_scrub = day03::find_rating(&lines, BitCriteria::LeastCommon)?;
    println!(
        "Part 2: Oxygen Generator rating {}, CO2 Scrubber rating {}. Life support rating: {}.",
        oxygen_gen,
        co2_scrub,
        oxygen_gen * co2_scrub
    );

    Ok(())
}
