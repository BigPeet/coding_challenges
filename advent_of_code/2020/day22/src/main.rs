use day22::{CardGame, CombatGame, Deck, RecursiveCombat};
use parsing::{InputError, ParsingResult};

fn main() -> ParsingResult {
    let text = parsing::filepath_from_args(std::env::args().collect()).and_then(parsing::read)?;

    // Part 1
    let (deck_a, deck_b) = text
        .split_once("\n\n")
        .ok_or(InputError::ParseGeneral)
        .and_then(|(l, r)| Ok((l.parse::<Deck>()?, r.parse::<Deck>()?)))?;

    let mut game = CombatGame::new(deck_a.clone(), deck_b.clone());
    let _ = game.play();
    println!(
        "Part 1: The final winning score of Combat is {}.",
        game.score()
    );

    // Part 2
    let mut game = RecursiveCombat::new(deck_a, deck_b);
    let _ = game.play();
    println!(
        "Part 2: The final winning score of RecursiveCombat is {}.",
        game.score()
    );

    Ok(())
}
