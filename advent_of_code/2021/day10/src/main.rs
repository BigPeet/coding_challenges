use day10::SymbolLine;
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let symbol_lines: Vec<SymbolLine> = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    let mut total_score = 0;
    for sl in symbol_lines.iter() {
        if let Some(sym) = sl.find_illegal() {
            total_score += sym.score();
        }
    }
    println!("Part 1: The total syntax error score is {}.", total_score);

    Ok(())
}
