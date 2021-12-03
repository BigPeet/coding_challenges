use parsing::InputError;

pub enum BitCriteria {
    MostCommon,
    LeastCommon,
}

pub fn count_bits(bit_lines: &[String]) -> Result<(u32, u32), InputError> {
    let total_lines = bit_lines.len();
    let columns = bit_lines.first().ok_or(InputError::ParseGeneral)?.len();
    let mut counts = vec![0; columns];
    for line in bit_lines.iter() {
        for (i, c) in line.chars().enumerate() {
            if c == '1' {
                counts[i] += 1;
            }
        }
    }

    let half = total_lines / 2;
    let mut gamma = 0;
    let mut epsilon = 0;
    counts.iter().enumerate().for_each(|(i, count)| {
        let add = 1 << (columns - 1 - i);
        if *count > half {
            // 1 was most common bit
            gamma += add;
        } else {
            // 1 was least common bit
            epsilon += add;
        }
    });

    Ok((gamma, epsilon))
}

fn count_bits_by_index(bit_lines: &[String], remaining: &[usize], index: usize) -> usize {
    let mut count = 0;
    for i in remaining {
        if let Some(c) = bit_lines[*i].chars().nth(index) {
            if c == '1' {
                count += 1;
            }
        }
    }
    count
}

fn filter_by_criteria(
    bit_lines: &[String],
    remaining: &mut Vec<usize>,
    index: usize,
    criteria: &BitCriteria,
) {
    let total = remaining.len();
    let half = total / 2 + total.rem_euclid(2);
    let mut value = if count_bits_by_index(bit_lines, remaining, index) >= half {
        1
    } else {
        0
    };
    if matches!(criteria, BitCriteria::LeastCommon) {
        value ^= 1; // invert 0 -> 1 or vice versa
    }
    let value = if value == 1 { '1' } else { '0' };
    remaining.retain(|id| {
        if let Some(bit_value) = bit_lines[*id].chars().nth(index) {
            bit_value == value
        } else {
            false
        }
    })
}

pub fn find_rating(bit_lines: &[String], criteria: BitCriteria) -> Result<u32, InputError> {
    let mut remaining: Vec<usize> = (0..bit_lines.len()).collect();
    let columns = bit_lines.first().unwrap_or(&String::default()).len();
    let mut index = 0;
    while !remaining.is_empty() && remaining.len() > 1 && index < columns {
        filter_by_criteria(bit_lines, &mut remaining, index, &criteria);
        index += 1;
    }
    if remaining.len() == 1 {
        Ok(u32::from_str_radix(&bit_lines[remaining[0]], 2)?)
    } else {
        Err(InputError::ParseGeneral)
    }
}
