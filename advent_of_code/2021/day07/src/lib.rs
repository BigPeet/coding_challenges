pub fn median(numbers: &mut [u32]) -> u32 {
    numbers.sort_unstable();
    let mid = numbers.len() / 2;
    let mut median = numbers[mid];
    if numbers.len().rem_euclid(2) == 0 {
        median += numbers[mid - 1];
        median /= 2;
    }
    median
}

fn abs_diff(a: u32, b: u32) -> u32 {
    if a > b {
        a - b
    } else {
        b - a
    }
}

pub fn fuel_cost(numbers: &[u32], target: u32) -> u32 {
    numbers
        .iter()
        .fold(0, |acc, elem| acc + abs_diff(*elem, target))
}
