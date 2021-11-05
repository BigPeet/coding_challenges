use day02::Policy;
use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let lines = parsing::filepath_from_args(env::args().collect()).and_then(parsing::get_lines)?;
    let valid: Vec<bool> = lines
        .iter()
        .filter_map(|line| {
            line.split_once(":")
                .and_then(|(l, r)| l.parse::<Policy>().map(|pol| pol.validate_p1(r)).ok())
        })
        .collect();

    let count = valid.iter().filter(|v| **v).count();
    println!("Valid password: {}", count);

    Ok(())
}
