use day05::{Line, Point};
use parsing::ParsingResult;
use std::collections::HashMap;

fn main() -> ParsingResult {
    let lines: Vec<Line> = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    let mut point_map: HashMap<Point, usize> = HashMap::new();
    for l in lines.iter().filter(|l| !l.is_diagonal()) {
        for p in l.points() {
            point_map.entry(p).and_modify(|e| *e += 1).or_insert(1);
        }
    }
    println!(
        "Part 1: There are {} points where at least 2 lines overlap.",
        point_map.values().filter(|v| **v > 1).count()
    );

    // Part 2
    let mut point_map: HashMap<Point, usize> = HashMap::new();
    for l in lines.iter() {
        for p in l.points() {
            point_map.entry(p).and_modify(|e| *e += 1).or_insert(1);
        }
    }
    println!(
        "Part 2: There are {} points where at least 2 lines overlap.",
        point_map.values().filter(|v| **v > 1).count()
    );

    Ok(())
}
