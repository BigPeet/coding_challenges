pub fn count_any(group: &[String]) -> usize {
    let mut arr: [bool; 26] = [false; 26];
    for line in group.iter() {
        for b in line.bytes() {
            let index = b - b'a';
            arr[index as usize] = true;
        }
    }
    arr.iter().filter(|x| **x).count()
}

pub fn count_all(group: &[String]) -> usize {
    let mut arr: [usize; 26] = [0; 26];
    for line in group.iter() {
        for b in line.bytes() {
            let index = b - b'a';
            arr[index as usize] += 1;
        }
    }
    let group_len = group.len();
    arr.iter().filter(|x| **x == group_len).count()
}
