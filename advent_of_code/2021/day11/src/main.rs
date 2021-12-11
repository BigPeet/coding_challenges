use day11::OctoGrid;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let mut grid = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::read)?
        .parse::<OctoGrid>()?;

    // Part 1
    let mut total_flashes = 0;
    let steps = 100;
    for _ in 0..steps {
        total_flashes += grid.step();
    }
    println!(
        "Part 1: After {} steps there have been {} total flashes.",
        steps, total_flashes
    );

    Ok(())
}
