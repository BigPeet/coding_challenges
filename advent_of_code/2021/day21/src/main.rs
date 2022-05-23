use day21::DiracGame;
use parsing::{InputError, ParsingResult};

fn parse_start(line: &str) -> Result<u32, InputError> {
    line.split_whitespace()
        .last()
        .and_then(|s| s.parse::<u32>().ok())
        .ok_or(InputError::ParseGeneral)
}

fn main() -> ParsingResult {
    let lines =
        parsing::filepath_from_args(std::env::args().collect()).and_then(parsing::get_lines)?;

    let start1 = parse_start(&lines[0])?;
    let start2 = parse_start(&lines[1])?;

    // Part 1
    let mut game = DiracGame::with_deterministic_die(start1, start2);
    game.play();

    println!(
        "Game Over. Losing player had {} points and the die has been rolled {} times. Result = {}",
        game.loser_score(),
        game.total_rolls(),
        game.loser_score() * game.total_rolls()
    );

    Ok(())
}
