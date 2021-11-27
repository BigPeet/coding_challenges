use day17::{Coordinate3d, Coordinate4d, CubeMap};
use parsing::ParsingResult;
use std::env;

fn main() -> ParsingResult {
    let lines = parsing::filepath_from_args(env::args().collect()).and_then(parsing::get_lines)?;
    let mut map = CubeMap::<Coordinate3d>::new(&lines)?;

    // Part 1
    for _ in 1..=6 {
        map.cycle();
    }
    println!(
        "Part 1: After 6 cycles, there are {} alive cells.",
        map.alive_cells()
    );

    // Part 2
    let mut hypermap = CubeMap::<Coordinate4d>::new(&lines)?;
    for _ in 1..=6 {
        hypermap.cycle();
    }
    println!(
        "Part 2: After 6 cycles, there are {} alive cells.",
        hypermap.alive_cells()
    );

    Ok(())
}
