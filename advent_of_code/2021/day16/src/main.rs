use parsing::ParsingResult;
use day16::Package;

fn main() -> ParsingResult {
    let hex_string =
        parsing::filepath_from_args(std::env::args().collect()).and_then(parsing::read)?;

    let hex_string = hex_string.trim_end().to_string();
    let bits = day16::str_to_bits(&hex_string)?;

    // Part 1
    let pack = Package::new(&bits)?;
    println!(
        "Part 1: The sum of all version IDs is {}.",
        pack.version_check_sum()
    );

    // Part 2
    println!("Part 2: The evaluation result is {}.", pack.evaluate()?);

    Ok(())
}
