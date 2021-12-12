use day12::CaveSystem;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let cave_system = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::read)?
        .parse::<CaveSystem>()?;

    // Part 1
    let paths = cave_system.find_all_paths();
    println!(
        "Part 1: There are {} paths through the cave system which visit small caves at most once.",
        paths.len()
    );

    Ok(())
}
