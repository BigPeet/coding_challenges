use day19::{Position, Scanner};
use parsing::{InputError, ParsingResult};

fn main() -> ParsingResult {
    let line_groups = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::get_line_groups)?;

    // Parsing
    let mut scanners = vec![];
    for group in line_groups.iter() {
        let mut beacons = Vec::with_capacity(group.len() - 1);
        let id = group[0]
            .split_whitespace()
            .nth(2)
            .ok_or(InputError::ParseGeneral)?
            .parse::<usize>()?;
        for l in group.iter().skip(1) {
            beacons.push(l.parse::<Position>()?);
        }
        scanners.push(Scanner::new(id, beacons));
    }

    // Part 1
    let mut origin = scanners.remove(0);
    while let Some(overlapping) = scanners
        .iter()
        .find(|s| s.max_common_distances(&origin) >= 12)
    {
        // TODO: Find the overlapping beacons.
        // TODO: Use them to create the translation/rotation between the scanners.
        // TODO: Add the beacons to origin.
        // TODO: Remove the scanner from the list/set.
        break;
    }

    Ok(())
}
