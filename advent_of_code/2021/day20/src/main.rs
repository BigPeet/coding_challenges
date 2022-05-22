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

    // Part 1
    let enhanced_2 = input_img.enhance_twice(&algorithm).unwrap();
    println!(
        "Lit pixels after two enhancement steps: {}",
        enhanced_2.lit_pixels()
    );

    // Part 2
    let mut result_img = enhanced_2;
    for _ in 0..24 {
        result_img = result_img.enhance_twice(&algorithm).unwrap();
    }
    println!(
        "Lit pixels after 50 enhancement steps: {}",
        result_img.lit_pixels()
    );

    Ok(())
}
