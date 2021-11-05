use std::cmp::Ordering;

fn find_two(
    vec: &[i32],
    mut high_idx: usize,
    mut low_idx: usize,
    target: i32,
) -> Option<(i32, i32)> {
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

// Assumes the passed in vector is sorted!
pub fn find_summands_p1(vec: &[i32], value: i32) -> Option<(i32, i32)> {
    find_two(vec, vec.len() - 1, 0, value)
}

// Assumes the passed in vector is sorted!
pub fn find_summands_p2(vec: &[i32], value: i32) -> Option<(i32, i32, i32)> {
    let mut fix_idx = 0;
    while fix_idx < vec.len() - 2 {
        let fix = vec[fix_idx];
        if let Some((a, b)) = find_two(vec, vec.len() - 1, fix_idx + 1, value - fix) {
            return Some((fix, a, b));
        }
        fix_idx += 1;
    }
    None
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn p1_test() {
        let mut v = vec![1721, 979, 366, 299, 675, 1456];
        v.sort_unstable();
        assert_eq!(Some((299, 1721)), find_summands_p1(&v, 2020));
    }

    #[test]
    fn p2_test() {
        let mut v = vec![1721, 979, 366, 299, 675, 1456];
        v.sort_unstable();
        assert_eq!(Some((366, 675, 979)), find_summands_p2(&v, 2020));
    }
}
