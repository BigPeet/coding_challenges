use day11::OctoGrid;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let mut grid = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::read)?
        .parse::<OctoGrid>()?;

    // Part 1
    let mut total_flashes = 0;
    let steps = 100;
    let mut first_full_flash = None; // for Part 2
    for step in 1..=steps {
        let flashes = grid.step();
        total_flashes += flashes;

        // for Part 2 (in case step < 100)
        if flashes == OctoGrid::size() && first_full_flash.is_none() {
            first_full_flash = Some(step);
        }
    }
    println!(
        "Part 1: After {} steps there have been {} total flashes.",
        steps, total_flashes
    );

    // Part 2
    let mut step = steps;
    while first_full_flash.is_none() {
        step += 1;
        if grid.step() == OctoGrid::size() {
            first_full_flash = Some(step);
        }
    }

    println!(
        "Part 2: The first step during which all octopuses flash is {}.",
        first_full_flash.unwrap()
    );

    Ok(())
}
