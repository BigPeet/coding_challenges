use day19::{Scanner, Vec3};
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
            beacons.push(l.parse::<Vec3>()?);
        }
        scanners.push(Scanner::new(id, beacons));
    }

    // Part 1
    let mut origin = scanners.remove(0);
    while !scanners.is_empty() {
        // Find scanner with overlapping beacons.
        let idx = scanners
            .iter()
            .position(|s| s.overlaps(&origin))
            .ok_or(InputError::ParseGeneral)?;
        // Use them to create the translation/rotation between the scanners.
        let overlapping = scanners.swap_remove(idx);
        //println!("Overlapping: {}", overlapping.id());
        let transformation = origin
            .try_transformation(&overlapping)
            .ok_or(InputError::ParseGeneral)?;

        //println!("Transformation: {:?}", transformation);
        // Add the beacons to origin.
        origin.combine(overlapping, transformation);
    }
    println!("Total Beacons: {}", origin.beacons());
    Ok(())
}
