use day16::{Ticket, TicketField};
use parsing::ParsingResult;
use std::collections::HashMap;
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
    let nearby_tickets: Vec<Ticket> = groups[2][1..]
        .iter()
        .filter_map(|line| line.parse::<Ticket>().ok())
        .collect();

    let mut invalid_values = Vec::new();
    for ticket in nearby_tickets.iter() {
        if let Some(num) = ticket.get_invalid(&fields) {
            invalid_values.push(*num);
        }
    }
    println!(
        "Part 1: The ticket scanning error rate is {}.",
        invalid_values.iter().sum::<u64>()
    );

    // Part 2
    // all have the same amount of positions
    let num_of_positions = nearby_tickets[0].positions.len();
    let valid_tickets: Vec<Ticket> = nearby_tickets
        .into_iter()
        .filter(|t| t.is_valid(&fields))
        .collect();

    // Find viable positions for each ticket field.
    let mut field_map: HashMap<&TicketField, Vec<usize>> = HashMap::new();
    for pos in 0..num_of_positions {
        for f in fields.iter() {
            // Are all values of this pos accepted by the field?
            if valid_tickets
                .iter()
                .map(|t| t.positions[pos])
                .all(|num| f.accepts(num))
            {
                if field_map.contains_key(f) {
                    field_map.get_mut(f).unwrap().push(pos);
                } else {
                    field_map.insert(f, vec![pos]);
                }
            }
        }
    }

    // Resolve
    let mut solution: Vec<(&TicketField, usize)> = Vec::new();
    while !field_map.is_empty() {
        let found_field: &TicketField;
        let found_value: usize;
        if let Some((field, values)) = field_map.iter().find(|(_, v)| v.len() == 1) {
            found_field = field;
            found_value = values[0];
        } else {
            panic!("Ambiguous solution!");
        }
        field_map.remove(found_field);
        field_map
            .values_mut()
            .for_each(|v| v.retain(|pos| *pos != found_value));
        solution.push((found_field, found_value));
    }

    let your_ticket = groups[1][1].parse::<Ticket>()?;
    let mut product = 1;
    for (field, pos) in solution.iter() {
        if field.name().starts_with("departure") {
            product *= your_ticket.positions[*pos];
        }
    }
    println!(
        "Part 2: The product of all departure fields is {}.",
        product
    );

    Ok(())
}
