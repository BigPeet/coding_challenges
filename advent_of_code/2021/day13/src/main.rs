use day13::{FoldInstruction, Point, TransparentPaper};
use parsing::InputError;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let groups = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::get_line_groups)?;

    if groups.len() != 2 {
        return Err(InputError::ParseGeneral);
    }

    let points = groups[0]
        .iter()
        .map(|l| l.parse::<Point>())
        .collect::<Result<Vec<Point>, InputError>>()?;
    let instructions = groups[1]
        .iter()
        .map(|l| l.parse::<FoldInstruction>())
        .collect::<Result<Vec<FoldInstruction>, InputError>>()?;

    // Part 1
    let mut paper = TransparentPaper::new(points);
    for instruction in instructions.iter().take(1) {
        paper = paper.apply(instruction);
    }
    println!(
        "Part 1: After applying the first fold, {} dots are visible.",
        paper.visible_dots()
    );

    // Part 2
    for instruction in instructions.iter().skip(1) {
        paper = paper.apply(instruction);
    }
    println!(
        "Part 2: After applying all instructions, the transparent paper looks like this:\n\n{}",
        paper
    );

    Ok(())
}
