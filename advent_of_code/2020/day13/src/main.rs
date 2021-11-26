use parsing::InputError;
use std::env;

fn part01(lines: &[String]) -> Result<(i32, i32), InputError> {
    let leave_time = lines[0].parse::<i32>()?;
    lines[1]
        .split(',')
        .filter_map(|s| s.parse::<i32>().ok())
        .map(|line_no| (line_no, day13::wait_time(leave_time, line_no)))
        .min_by_key(|(_, wait_time)| *wait_time)
        .ok_or(InputError::ParseGeneral)
}

fn part02<F>(lines: &[String], search_fn: F) -> i128
where
    F: Fn(Vec<(i128, i128)>) -> i128,
{
    let bus_lines: Vec<(i128, i128)> = lines[1]
        .split(',')
        .enumerate()
        .filter_map(|(i, line_no)| {
            if let Ok(parsed) = line_no.parse::<i128>() {
                Some((day13::wait_time(i as i32, parsed as i32) as i128, parsed))
            } else {
                None
            }
        })
        .collect();

    search_fn(bus_lines)
}

fn main() -> Result<(), InputError> {
    let lines = parsing::filepath_from_args(env::args().collect()).and_then(parsing::get_lines)?;

    // Part 1
    let res = part01(&lines)?;
    println!(
        "Part 1: The bus line {} has the lowest wait time: {}. Multiplication: {}",
        res.0,
        res.1,
        res.0 * res.1
    );

    // Part 2
    println!(
        "Part 2 v1: {} is the earliest timestamp (with sieve).",
        part02(&lines, day13::sieve)
    );
    println!(
        "Part 2 v2: {} is the earliest timestamp (by construction).",
        part02(&lines, day13::by_construction)
    );

    Ok(())
}
