use day22::{Cuboid, MegaCuboid, RebootStep, State};
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
        rs.apply_init(&mut grid, &area);
    }

    let init_active = grid.values().filter(|s| matches!(s, State::On)).count();
    println!(
        "Part 1: After the reboot steps, {} cubes are ON in the initialization area.",
        init_active
    );

    // Part 2
    let mut active_cuboids = MegaCuboid::default();
    for rs in reboot_steps.iter() {
        active_cuboids = rs.apply_reboot(active_cuboids);
    }
    let total_active = active_cuboids.volume();
    println!(
        "Part 2: After the reboot steps, {} cubes are ON in total.",
        total_active
    );

    Ok(())
}
