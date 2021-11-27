use day17::CubeMap;
use parsing::ParsingResult;
use std::env;

fn main() -> ParsingResult {
    let lines = parsing::filepath_from_args(env::args().collect()).and_then(parsing::get_lines)?;
    let mut map = CubeMap::new(&lines)?;

    // Part 1
    for _ in 1..=6 {
        map.cycle();
    }
    println!(
        "Part 1: After 6 cycles, there are {} alive cells.",
        map.alive_cells()
    );

    Ok(())
}
