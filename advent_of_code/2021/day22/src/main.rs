use day22::{Cuboid, RebootStep, State};
use parsing::ParsingResult;
use std::collections::HashMap;

fn main() -> ParsingResult {
    let reboot_steps: Vec<RebootStep> = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    let mut grid = HashMap::new();
    let area = Cuboid::new(-50, 50, -50, 50, -50, 50);
    for rs in reboot_steps.iter() {
        rs.apply(&mut grid, &area);
    }

    let cubes_on = grid.values().filter(|s| matches!(s, State::On)).count();
    println!("Part 1: After the reboot steps, {} cubes are on.", cubes_on);

    Ok(())
}
