use day11::SeatingPlan;
use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let lines: Vec<String> =
        parsing::filepath_from_args(env::args().collect()).and_then(parsing::get_lines)?;
    let mut plan = SeatingPlan::new(lines.as_slice());

    // Part 1
    while plan.step() {}
    println!(
        "Part 1: When stabilizing, there are {} occupied seats.",
        plan.count_occupied()
    );

    // Part 2
    let mut plan = SeatingPlan::new(lines.as_slice());
    plan.use_visibility = true;
    plan.occupied_limit = 5;
    while plan.step() {}
    println!("Part 2: When stabilizing with the modified visibility and rules, there are {} occupied seats.", plan.count_occupied());

    Ok(())
}
