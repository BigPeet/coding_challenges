use day22::CombatGame;
use day22::Deck;
use parsing::InputError;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let text = parsing::filepath_from_args(std::env::args().collect()).and_then(parsing::read)?;

    // Part 1
    let (deck_a, deck_b) = text
        .split_once("\n\n")
        .ok_or(InputError::ParseGeneral)
        .and_then(|(l, r)| Ok((l.parse::<Deck>()?, r.parse::<Deck>()?)))?;

    let mut game = CombatGame::new(deck_a, deck_b);
    while !game.is_over() {
        game.round();
    }
    println!("Part 1: The final winning score is {}.", game.score());

    Ok(())
}
