use parsing::InputError;
use std::env;

fn main() -> Result<(), InputError> {
    let lines = parsing::filepath_from_args(env::args().collect()).and_then(parsing::get_lines)?;
    let mut cur_pos = 0usize;
    let mut tree_count = 0usize;
    let line_length = lines[0].len(); // Assuming all lines have the same length!
    for line in lines.into_iter() {
        if line.chars().nth(cur_pos) == Some('#') {
            tree_count += 1;
        }
        cur_pos = (cur_pos + 3) % line_length;
    }
    println!("Part 1: Encountered {} trees.", tree_count);

    Ok(())
}
