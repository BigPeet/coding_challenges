use parsing::{InputError, ParsingResult};

fn main() -> ParsingResult {
    let lines =
        parsing::filepath_from_args(std::env::args().collect()).and_then(parsing::get_lines)?;

    let size = lines.len();
    let mut graph = Vec::with_capacity(size);
    for l in lines.iter() {
        graph.push(
            l.chars()
                .map(|c| c.to_digit(10).ok_or(InputError::ParseGeneral))
                .collect::<Result<Vec<u32>, InputError>>()?,
        );
    }

    // Part 1
    let start = (0, 0);
    let end = (size - 1, size - 1);
    let (_, total_risk) = day15::dijkstra_path(&graph, start, end);
    println!(
        "Part 1: The lowest total risk of any path from start to end is {}.",
        total_risk
    );

    // Part 2
    let extended_graph = day15::extend_graph(graph);
    let size = extended_graph.len();
    let start = (0, 0);
    let end = (size - 1, size - 1);
    let (_, total_risk) = day15::dijkstra_path(&extended_graph, start, end);
    println!(
        "Part 2: The lowest total risk of any extended path from start to end is {}.",
        total_risk
    );

    Ok(())
}
