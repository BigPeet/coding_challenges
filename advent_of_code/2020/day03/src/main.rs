use day03::{Counter, Slope};
use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let lines = parsing::filepath_from_args(env::args().collect()).and_then(parsing::get_lines)?;
    let line_length = lines[0].len(); // Assuming all lines have the same length!

    let mut slope_cnts: [Counter; 5] = [
        Counter::new(Slope::new(1, 1), line_length),
        Counter::new(Slope::new(3, 1), line_length),
        Counter::new(Slope::new(5, 1), line_length),
        Counter::new(Slope::new(7, 1), line_length),
        Counter::new(Slope::new(1, 2), line_length),
    ];

    for (line_no, line) in lines.into_iter().enumerate() {
        for cnt in slope_cnts.iter_mut() {
            cnt.update(&line, line_no);
        }
    }

    println!("Part 1: Encountered {} trees.", slope_cnts[1].count());
    println!(
        "Part 2: Encountered {} trees.",
        slope_cnts.iter().fold(1, |acc, c| acc * c.count())
    );

    Ok(())
}
