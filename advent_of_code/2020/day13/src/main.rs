use parsing::InputError;
use std::env;

fn wait_time(leave_time: i32, line_no: i32) -> i32 {
    let modulo = leave_time % line_no;
    if modulo != 0 {
        line_no - modulo
    } else {
        0
    }
}

fn part01(lines: &[String]) -> Result<(i32, i32), InputError> {
    let leave_time = lines[0].parse::<i32>()?;
    let mut bus_lines: Vec<i32> = lines[1]
        .split(',')
        .filter_map(|s| s.parse::<i32>().ok())
        .collect();
    bus_lines.sort_unstable_by_key(|line_no| wait_time(leave_time, *line_no));
    Ok((bus_lines[0], wait_time(leave_time, bus_lines[0])))
}

fn main() -> Result<(), InputError> {
    let lines = parsing::filepath_from_args(env::args().collect()).and_then(parsing::get_lines)?;

    let res = part01(&lines)?;
    println!(
        "Part 1: The bus line {} has the lowest wait time: {}. Multiplication: {}",
        res.0,
        res.1,
        res.0 * res.1
    );

    Ok(())
}
