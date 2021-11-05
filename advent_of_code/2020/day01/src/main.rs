use std::env;
use std::fs::File;
use std::io::prelude::*;
use std::io::BufReader;

#[derive(Debug)]
enum InputError {
    WrongNumberOfArgs,
    Parse(std::io::Error),
}

impl From<std::io::Error> for InputError {
    fn from(e: std::io::Error) -> InputError {
        InputError::Parse(e)
    }
}

fn parse_args() -> Result<String, InputError> {
    let args: Vec<String> = env::args().collect();
    if args.len() != 2 {
        return Err(InputError::WrongNumberOfArgs);
    }
    // TODO: is there a better way? Does this even avoid an allocation?
    if let Some(file_path) = args.into_iter().nth(1) {
        Ok(file_path)
    } else {
        Err(InputError::WrongNumberOfArgs)
    }
}

fn parse_input(path: String) -> Result<Vec<i32>, InputError> {
    let f = File::open(path)?;
    let f = BufReader::new(f);

    //TODO: Add error handling instead of silently dropping errors
    let mut numbers: Vec<i32> = f
        .lines()
        .into_iter()
        .filter_map(|l| l.ok())
        .filter_map(|l| l.parse::<i32>().ok())
        .collect();
    numbers.sort();
    Ok(numbers)
}

// Assumes the passed in vector is sorted!
fn find_summands(vec: &Vec<i32>, value: i32) -> Option<(i32, i32)> {
    let mut high_idx = vec.len() - 1;
    let mut low_idx = 0;

    while low_idx < high_idx {
        let high = vec[high_idx];
        let low = vec[low_idx];
        let sum = high + low;
        if sum == value {
            return Some((low, high));
        } else if sum > value {
            high_idx -= 1;
        } else {
            low_idx += 1;
        }
    }

    None
}

fn main() -> Result<(), InputError> {
    let numbers = parse_args().and_then(|path| parse_input(path))?;

    if let Some((a, b)) = find_summands(&numbers, 2020) {
        println!("Found entries: {} + {} = {}", a, b, 2020);
        println!("Multiplication: {} * {} = {}", a, b, a * b);
    } else {
        println!("No match found!");
    }

    Ok(())
}
