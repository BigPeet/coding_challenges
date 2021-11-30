use day20::Tile;
use parsing::{InputError, ParsingResult};
use std::env;

fn main() -> ParsingResult {
    let groups: Vec<String> = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::read)?
        .split("\n\n")
        .map(|sl| sl.to_owned())
        .collect();

    let tiles = groups
        .iter()
        .map(|g| g.parse::<Tile>())
        .collect::<Result<Vec<Tile>, InputError>>()?;

    let mut corner_ids = vec![];
    for t in tiles.iter() {
        let match_count = tiles.iter().filter(|ot| *ot != t && ot.matches(t)).count();
        if match_count == 2 {
            corner_ids.push(t.id());
        }
    }
    println!(
        "Part 1: The ids of the corner tiles are {:?}. Product: {}",
        corner_ids,
        corner_ids.iter().product::<usize>()
    );

    Ok(())
}
