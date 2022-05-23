use day21::{DeterministicDie, Die, DiracGame};
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
    let mut det_die = DeterministicDie::default();
    let (_, loser_score) =
        DiracGame::part1(start1, start2).play_with_deterministic_die(&mut det_die);

    println!(
        "Part 1: Losing player had {} points and the die has been rolled {} times. Result = {}",
        loser_score,
        det_die.total_rolls(),
        loser_score * det_die.total_rolls()
    );

    // Part 2
    let (max_dim, _) = DiracGame::part2(start1, start2).play_with_dirac_die();
    println!("Part 2: Higher number of universes won: {}", max_dim);

    Ok(())
}
