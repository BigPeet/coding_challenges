pub fn wait_time(leave_time: i32, line_no: i32) -> i32 {
    let modulo = leave_time % line_no;
    if modulo != 0 {
        line_no - modulo
    } else {
        0
    }
}

pub fn sieve(mut bus_lines: Vec<(i128, i128)>) -> i128 {
    // see https://en.wikipedia.org/wiki/Chinese_remainder_theorem#Search_by_sieving
    bus_lines.sort_unstable_by(|(_, a), (_, b)| b.cmp(a));
    let mut i: i128 = 0;
    let mut delta: i128 = 1;
    for (offset, line_no) in bus_lines.iter() {
        while i % line_no != *offset {
            i += delta;
        }
        delta *= *line_no;
    }
    i
}

fn mod_inverse(a: i128, n: i128) -> Option<i128> {
    // see https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm#Modular_integers
    let mut t = 0;
    let mut new_t = 1;
    let mut r = n;
    let mut new_r = a;

    while new_r != 0 {
        let quotient = r / new_r;
        let t_tmp = t;
        t = new_t;
        new_t = t_tmp - quotient * new_t;

        let r_tmp = r;
        r = new_r;
        new_r = r_tmp - quotient * new_r;
    }

    if r > 1 {
        // a is not invertible
        return None;
    }
    if t < 0 {
        t += n;
    }

    Some(t)
}

pub fn by_construction(bus_lines: Vec<(i128, i128)>) -> i128 {
    // see https://en.wikipedia.org/wiki/Chinese_remainder_theorem
    // see https://brilliant.org/wiki/chinese-remainder-theorem
    let n = bus_lines
        .iter()
        .map(|(_, line_no)| *line_no)
        .reduce(|a, b| a * b)
        .expect("Reduce failed!");

    let mut sum = 0;
    for (offset, line_no) in bus_lines.iter() {
        let yi = n / *line_no;
        let zi =
            mod_inverse(yi, *line_no).unwrap_or_else(|| panic!("yi = {} is not invertible!", yi));
        sum += offset * yi * zi;
    }
    sum % n
}
