pub fn separation_points(numbers: &[usize]) -> Vec<usize> {
    let mut separators = Vec::new();
    for i in 0..numbers.len() - 1 {
        let diff = numbers[i + 1] - numbers[i];
        if diff == 3 {
            separators.push(i + 1);
        }
    }
    separators
}

pub fn calculate_combinations(numbers: &[usize]) -> usize {
    if numbers.len() < 3 {
        1
    } else {
        // see notes.md for explanation
        let k: u32 = (numbers.len() - 2) as u32;
        let base: u32 = 2;
        let total_combinations = base.pow(k);
        let mut invalid_combinations = 0;

        if k > 2 {
            // there are invalid combinations to handle
            if k == 3 {
                invalid_combinations = 1;
            } else {
                let m: u32 = k - 2;
                invalid_combinations = m * base.pow(m - 1) - (m - 1) * base.pow(m - 2);
            }
        }

        (total_combinations - invalid_combinations) as usize
    }
}
