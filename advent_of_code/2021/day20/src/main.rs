use day20::{Algorithm, Image};
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let line_groups = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::get_line_groups)?;

    let algorithm = line_groups[0][0].parse::<Algorithm>()?;
    let input_img = line_groups[1]
        .iter()
        .flat_map(|s| s.chars().chain("\n".chars()))
        .collect::<String>()
        .parse::<Image>()?;

    let enhanced_2 = input_img.enhance_twice(&algorithm).unwrap();
    println!(
        "Lit pixels after two enhancement steps: {}",
        enhanced_2.lit_pixels()
    );

    Ok(())
}
