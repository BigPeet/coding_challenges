use std::cmp::Ordering;

// Taken from day01 and slightly modified
fn find_two(
    vec: &[usize],
    mut high_idx: usize,
    mut low_idx: usize,
    target: usize,
) -> Option<(usize, usize)> {
    while low_idx < high_idx {
        let high = vec[high_idx];
        let low = vec[low_idx];
        let sum = high + low;
        match sum.cmp(&target) {
            Ordering::Equal => return Some((low, high)),
            Ordering::Greater => high_idx -= 1,
            Ordering::Less => low_idx += 1,
        };
    }
    None
}

pub fn find_invalid_number(numbers: &[usize], preamble_length: usize) -> Option<usize> {
    for (index, num) in numbers[preamble_length..].iter().enumerate() {
        let num_index = index + preamble_length;

        let mut candidates = numbers[num_index - preamble_length..num_index].to_vec();
        // required, if we want to reuse the find_two function above
        // Alternatively, a naive O(n^2) search could be performed on the slice.
        // Trade-off: allocating new vector vs. faster search speed
        candidates.sort_unstable();
        if find_two(&candidates, preamble_length - 1, 0, *num).is_none() {
            return Some(*num);
        }
    }
    None
}

pub fn find_sum_range(numbers: &[usize], target: usize) -> Option<(usize, usize)> {
    for (index_start, start) in numbers.iter().enumerate() {
        let mut sum = *start;
        for (index_end, num) in numbers[index_start + 1..].iter().enumerate() {
            sum += *num;
            match sum.cmp(&target) {
                Ordering::Equal => return Some((index_start, index_end + index_start + 1)),
                Ordering::Greater => break,
                _ => (),
            };
        }
    }
    None
}
