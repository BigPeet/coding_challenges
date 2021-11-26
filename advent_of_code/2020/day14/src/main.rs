use day14::{Operation, OperationV2, Program, ProgramV2};
use parsing::ParsingResult;
use std::env;

fn main() -> ParsingResult {
    let lines = parsing::filepath_from_args(env::args().collect()).and_then(parsing::get_lines)?;

    // Part 1
    let mut prog = Program::new();
    for line in lines.iter() {
        let op = line.parse::<Operation>()?;
        prog.apply(op);
    }
    println!(
        "Part 1: The sum of all values left in memory is {}.",
        prog.sum()
    );

    // Part 2
    let mut prog = ProgramV2::new();
    for line in lines.iter() {
        let op = line.parse::<OperationV2>()?;
        prog.apply(op);
    }
    println!(
        "Part 2: The sum of all values left in memory is {}.",
        prog.sum()
    );

    Ok(())
}
