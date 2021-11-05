use std::cmp::Ordering;
use std::env;
use std::fs::File;
use std::io::prelude::*;
use std::io::BufReader;
use std::num::ParseIntError;

#[derive(Debug)]
enum InputError {
    WrongNumberOfArgs,
    IO(std::io::Error),
    Parse(ParseIntError),
}

impl From<std::io::Error> for InputError {
    fn from(e: std::io::Error) -> InputError {
        InputError::IO(e)
    }
}

impl From<ParseIntError> for InputError {
    fn from(e: ParseIntError) -> InputError {
        InputError::Parse(e)
    }
}

fn parse_args() -> Result<String, InputError> {
    let mut args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        return Err(InputError::WrongNumberOfArgs);
    }
    Ok(args.swap_remove(1))
}

fn parse_input(path: String) -> Result<Vec<i32>, InputError> {
    let f = File::open(path)?;
    let f = BufReader::new(f);

    let mut numbers = f
        .lines()
        .into_iter()
        .map(|l| l.map_err(InputError::IO))
        .map(|l| l.and_then(|l| l.parse::<i32>().map_err(InputError::Parse)))
        .collect::<Result<Vec<i32>, InputError>>()?;
    numbers.sort_unstable();
    Ok(numbers)
}

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
fn find_summands_p1(vec: &[i32], value: i32) -> Option<(i32, i32)> {
    find_two(vec, vec.len() - 1, 0, value)
}

// Assumes the passed in vector is sorted!
fn find_summands_p2(vec: &[i32], value: i32) -> Option<(i32, i32, i32)> {
    let mut fix_idx = 0;
    while fix_idx < vec.len() - 2 {
        let fix = vec[fix_idx];
        if let Some((a, b)) = find_two(vec, vec.len() - 1, fix_idx + 1, value - fix) {
            return Some((a, b, fix));
        }
        fix_idx += 1;
    }
    None
}

fn main() -> Result<(), InputError> {
    let numbers = parse_args().and_then(parse_input)?;

    println!("Part 1:");
    if let Some((a, b)) = find_summands_p1(&numbers, 2020) {
        println!("Found entries: {} + {} = {}", a, b, 2020);
        println!("Multiplication: {} * {} = {}", a, b, a * b);
    } else {
        println!("No match found!");
    }
    println!("Part 2:");
    if let Some((a, b, c)) = find_summands_p2(&numbers, 2020) {
        println!("Found entries: {} + {} + {} = {}", a, b, c, 2020);
        println!("Multiplication: {} * {} * {} = {}", a, b, c, a * b * c);
    } else {
        println!("No match found!");
    }

    Ok(())
}
