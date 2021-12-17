use day17::Area;
use day17::Position;
use day17::Probe;
use day17::Vec2;
use day17::Velocity;
use parsing::ParsingResult;
use std::collections::HashSet;

fn main() -> ParsingResult {
    let area = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::read)?
        .parse::<Area>()?;

    // Part 1
    let mut viable_velocities = HashSet::new();
    let (x_from, x_to) = area.velocity_x_values(); // x_from < x_to
    for x in x_from..=x_to {
        for s in area.hit_candidates(x) {
            let mut y = area.lower_bound();
            let mut has_hit = false;
            let mut has_missed_again = false;
            while !has_hit || !has_missed_again {
                let vel = Velocity::new(x, y);
                let mut probe = Probe::new(vel);
                probe.steps(s);
                let mut hit = probe.pos().is_in_area(&area);
                if !hit && probe.vel().0.x == 0 {
                    // Can it still fall into the area?
                    // We know it is and will stay in the right horizontal bounds.
                    while !probe.pos().has_missed(&area) && !hit {
                        probe.step();
                        hit = probe.pos().is_in_area(&area);
                    }
                }
                if has_hit && !hit {
                    // We are now aiming to high.
                    has_missed_again = true;
                }
                if hit {
                    println!("Can hit target: {:?}", vel);
                    has_hit = true;
                    viable_velocities.insert(vel);
                }
                y += 1;
            }
        }
    }
    println!("Found {} velocities.", viable_velocities.len());

    Ok(())
}
