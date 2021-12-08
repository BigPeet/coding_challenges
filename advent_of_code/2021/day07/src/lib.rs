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

pub fn mean(numbers: &[u32]) -> f64 {
    let sum = numbers.iter().sum::<u32>();
    let len = numbers.len();
    sum as f64 / len as f64
}

pub fn abs_diff(a: u32, b: u32) -> u32 {
    if a > b {
        a - b
    } else {
        b - a
    }
}

pub fn crab_diff(a: u32, b: u32) -> u32 {
    let abs_diff = abs_diff(a, b);
    (1..=abs_diff).sum()
}

pub fn fuel_cost(numbers: &[u32], target: u32, diff: fn(u32, u32) -> u32) -> u32 {
    numbers
        .iter()
        .fold(0, |acc, elem| acc + diff(*elem, target))
}
