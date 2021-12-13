use day23::CupsGame;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let mut cups_game = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::read)?
        .parse::<CupsGame>()?;

    let rounds = 100;
    for _ in 1..=rounds {
        cups_game.play_move();
    }
    println!(
        "Part 1: After playing {} moves, the ordering after 1 is: {}.",
        rounds,
        cups_game.score()
    );

    Ok(())
}
