use day23::Burrow;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let burrow = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::read)?
        .parse::<Burrow>()?;

    println!("{}", burrow);

    Ok(())
}
