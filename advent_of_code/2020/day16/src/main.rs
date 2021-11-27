use day16::TicketField;
use parsing::ParsingResult;
use std::env;

fn main() -> ParsingResult {
    let groups = parsing::filepath_from_args(env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::get_line_groups)?;

    // FIXME: is there a way to introduce some error handling here and below?
    let fields: Vec<TicketField> = groups[0]
        .iter()
        .filter_map(|line| line.parse::<TicketField>().ok())
        .collect();

    // Part 1

    // Parse nearby tickets, skip "header line".
    let nearby_tickets: Vec<Vec<i32>> = groups[2][1..]
        .iter()
        .map(|line| {
            line.split(',')
                .filter_map(|tok| tok.parse::<i32>().ok())
                .collect::<Vec<i32>>()
        })
        .collect();

    let mut invalid_values = Vec::new();
    for ticket in nearby_tickets.iter() {
        for num in ticket.iter() {
            if fields.iter().all(|f| !f.accepts(*num)) {
                invalid_values.push(*num);
            }
        }
    }
    println!(
        "Part 1: The ticket scanning error rate is {}",
        invalid_values.iter().sum::<i32>()
    );

    Ok(())
}
