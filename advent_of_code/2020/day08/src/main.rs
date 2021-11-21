use day08::{Instruction, Program};
use parsing::InputError;
use std::env;

fn run_until_loop_or_terminates(prog: &mut Program) -> Vec<usize> {
    let mut history = Vec::new();
    let mut last_ic = 0;
    while !history.contains(&last_ic) && !prog.sucess() && prog.is_running() {
        history.push(last_ic);
        last_ic = prog.step();
    }
    history
}

fn main() -> Result<(), InputError> {
    let instructions: Vec<Instruction> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(|lines| {
            lines
                .into_iter()
                .map(|line| line.parse::<Instruction>())
                .collect()
        })?;

    let mut prog = Program::new(instructions);

    // Part 1
    let history = run_until_loop_or_terminates(&mut prog);
    println!(
        "Part 1: Accumulator value before executing first instruction twice: {}",
        prog.acc_value()
    );

    // Part 2
    prog.reset();
    let mut final_acc_value = 0;
    let mut switched_inst = 0;
    for ic in history.iter().rev() {
        // Is the instruction a JMP or NOP => Switch it and test modified program.
        match prog.get_instruction(*ic) {
            Instruction::JMP(_) | Instruction::NOOP(_) => {
                let mut copy = prog.clone();
                copy.switch_instruction(*ic);
                run_until_loop_or_terminates(&mut copy);
                if copy.sucess() {
                    switched_inst = *ic;
                    final_acc_value = copy.acc_value();
                    break;
                }
            }
            _ => (),
        }
    }
    println!(
        "Part 2: Switched instruction '{}' at index {}. Accumulator value after termination: {}",
        prog.get_instruction(switched_inst),
        switched_inst,
        final_acc_value
    );

    Ok(())
}
