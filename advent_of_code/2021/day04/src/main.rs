use day04::BingoBoard;
use parsing::ParsingResult;
use std::num::ParseIntError;

fn play<'a>(drawn_numbers: &[u32], boards: &'a mut [BingoBoard]) -> Option<(&'a BingoBoard, u32)> {
    let mut winner_idx = None;
    let mut finished = false;
    for num in drawn_numbers {
        for (idx, board) in boards.iter_mut().enumerate() {
            board.mark(*num);
            if board.finished() {
                winner_idx = Some((idx, num));
                finished = true;
                break;
            }
        }
        if finished {
            break;
        }
    }
    if let Some((idx, num)) = winner_idx {
        Some((&boards[idx], *num))
    } else {
        None
    }
}

fn main() -> ParsingResult {
    let groups = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::get_line_groups)?;

    // Parsing
    let drawn = groups[0][0]
        .split(',')
        .map(|s| s.parse::<u32>())
        .collect::<Result<Vec<u32>, ParseIntError>>()?;

    let mut boards = vec![];
    for group in groups[1..].iter() {
        let s = group.join("\n");
        boards.push(s.parse::<BingoBoard>()?);
    }

    // Part 1
    if let Some((board, num)) = play(&drawn, &mut boards) {
        let unmarked = board.unmarked();
        println!(
            "Part 1: Board won. Sum of unmarked numbers: {}. Last number: {}. Final Score: {}",
            unmarked,
            num,
            num * unmarked
        );
    }

    Ok(())
}
