use day12::CaveSystem;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let cave_system = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::read)?
        .parse::<CaveSystem>()?;

    // Part 1
    let paths = cave_system.find_all_paths(CaveSystem::normal_path_filter);
    println!(
        "Part 1: There are {} paths through the cave system which visit small caves at most once.",
        paths.len()
    );

    // Part 2
    let paths = cave_system.find_all_paths(CaveSystem::advanced_path_filter);
    println!(
        "Part 2: There are {} paths through the cave system which visit at most a single small cave twice.",
        paths.len()
    );

    Ok(())
}
