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
    if numbers.len() < 2 {
        1
    } else {
        // start and end are fixed.
        // for every other point, there are two options: use it or not use it
        let base: usize = 2;
        let mut combinations = base.pow((numbers.len() - 2) as u32);

        // If the difference between start and end is > 3, then some points are required and
        // can not be left out.
        // Remove these invalid combinations.
        // FIXME: not sure if this works for ALL inputs...
        let range_delta = numbers[numbers.len() - 1] - numbers[0];
        if range_delta > 3 {
            combinations -= range_delta / 3;
        }
        combinations
    }
}
