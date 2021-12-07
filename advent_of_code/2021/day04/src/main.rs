use day04::BingoBoard;
use parsing::ParsingResult;
use std::num::ParseIntError;

fn play(
    numbers: &[u32],
    already_drawn: usize,
    boards: &mut [BingoBoard],
) -> Option<(usize, usize)> {
    let mut winner = None;
    let mut finished = false;
    for (num_idx, num) in numbers.iter().enumerate().skip(already_drawn) {
        for (board_idx, board) in boards.iter_mut().enumerate() {
            board.mark(*num);
            if board.finished() {
                winner = Some((board_idx, num_idx));
                finished = true;
                break;
            }
        }
        if finished {
            break;
        }
    }
    winner
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
    let mut start = 0;
    if let Some((bidx, nidx)) = play(&drawn, start, &mut boards) {
        let unmarked = boards[bidx].unmarked();
        println!(
            "Part 1: Sum of unmarked numbers: {}. Last number: {}. Final Score: {}",
            unmarked,
            drawn[nidx],
            drawn[nidx] * unmarked
        );

        boards.swap_remove(bidx); // for Part 2
        start = nidx;
    }

    // Part 2
    while let Some((bidx, nidx)) = play(&drawn, start, &mut boards) {
        if boards.len() == 1 {
            let unmarked = boards[bidx].unmarked();
            println!(
                "Part 2: Sum of unmarked numbers: {}. Last number: {}. Final Score: {}",
                unmarked,
                drawn[nidx],
                drawn[nidx] * unmarked
            );
            break;
        } else {
            start = nidx;
            boards.swap_remove(bidx);
        }
    }
    Ok(())
}
