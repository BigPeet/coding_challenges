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

    // Part 2
    let mut basin_sizes = hmap.basins();
    basin_sizes.sort_unstable_by(|a, b| b.cmp(a)); // sort by descending order
    println!(
        "Part 2: Multiplying the sizes of the three largest basins {:?} results in {}.",
        &basin_sizes[0..3],
        &basin_sizes[0..3].iter().product::<u32>()
    );

    Ok(())
}
