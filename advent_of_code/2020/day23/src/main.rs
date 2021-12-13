use day23::{CupsGame, RealCupsGame};
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let input_string =
        parsing::filepath_from_args(std::env::args().collect()).and_then(parsing::read)?;

    // Part 1
    let mut game = input_string.parse::<CupsGame>()?;
    let rounds = 100;
    for _ in 1..=rounds {
        game.play_move();
    }
    println!(
        "Part 1: After playing {} moves, the ordering after 1 is: {}.",
        rounds,
        game.score()
    );

    // Part 2
    let mut game = input_string.parse::<RealCupsGame>()?;
    let rounds = 10_000_000;
    for _ in 1..=rounds {
        game.play_move();
    }
    println!(
        "Part 2: After playing {} moves, the product of the two cups after 1 is: {}.",
        rounds,
        game.score()
    );

    Ok(())
}
