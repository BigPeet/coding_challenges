use day24::{TileMap, TileReference};
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let references: Vec<TileReference> = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    let mut tmap = TileMap::default();
    for tref in references.iter() {
        tmap.flip_tile(tref);
    }
    let black_tiles = tmap.black_tiles();
    println!(
        "Part 1: The number of black tiles is {}.",
        black_tiles.len()
    );

    Ok(())
}
