pub type ValueType = u64;

pub fn transform(subject_number: ValueType, loop_size: usize) -> ValueType {
    let mut val = 1;
    (0..loop_size).for_each(|_| val = loop_step(val, subject_number));
    val
}

fn loop_step(val: ValueType, subject_number: ValueType) -> ValueType {
    (val * subject_number).rem_euclid(20201227)
}

pub fn calculate_loop_size(pub_key: ValueType) -> usize {
    let mut loop_size = 0;
    let mut val = 1;
    let subject_number = 7;
    while val != pub_key {
        val = loop_step(val, subject_number);
        loop_size += 1;
    }
    loop_size
}
