use day20::{TileImage, Match, Tile, Image};
use parsing::{InputError, ParsingResult};
use std::collections::HashMap;
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

    // Part 1
    let mut match_map: HashMap<usize, Vec<Match>> = HashMap::new();
    for t in tiles.iter() {
        let matches: Vec<Match> = tiles.iter().filter_map(|ot| t.matches(ot)).collect();
        match_map.insert(t.id(), matches);
    }
    let mut corner_ids = vec![];
    for (id, matches) in match_map.iter() {
        if matches.len() == 2 {
            corner_ids.push(*id);
        }
    }
    println!(
        "Part 1: The ids of the corner tiles are {:?}. Product: {}",
        corner_ids,
        corner_ids.iter().product::<usize>()
    );

    // Part 2
    let mut tile_map: HashMap<usize, Tile> = HashMap::new();
    for t in tiles.into_iter() {
        tile_map.insert(t.id(), t);
    }

    let tile_image = TileImage::assemble(tile_map, match_map)?;
    println!("IDs in assemble form:\n{}", tile_image);

    let image = Image::from(tile_image);
    println!("The image:");
    println!("{}", image);

    Ok(())
}
