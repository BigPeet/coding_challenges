use std::fs::File;
use std::io::prelude::*;
use std::io::BufReader;
use std::num::ParseIntError;

#[derive(Debug)]
pub enum InputError {
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

pub fn filepath_from_args(mut args: Vec<String>) -> Result<String, InputError> {
    if args.len() != 2 {
        return Err(InputError::WrongNumberOfArgs);
    }
    Ok(args.swap_remove(1))
}

pub fn get_lines(path: String) -> Result<Vec<String>, InputError> {
    let f = File::open(path)?;
    let f = BufReader::new(f);
    f.lines()
        .into_iter()
        .map(|l| l.map_err(InputError::IO))
        .collect()
}

pub fn list_of_numbers(lines: Vec<String>) -> Result<Vec<i32>, InputError> {
    let numbers = lines
        .into_iter()
        .map(|l| l.parse::<i32>())
        .collect::<Result<Vec<i32>, ParseIntError>>()?;
    Ok(numbers)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn filepath_from_args_test_too_few_args() {
        let v: Vec<String> = vec![];
        assert!(filepath_from_args(v).is_err());
        let v: Vec<String> = vec!["only executable name".to_string()];
        assert!(filepath_from_args(v).is_err());
    }

    #[test]
    fn filepath_from_args_test_valid_input() {
        let v: Vec<String> = vec!["1", "input"].into_iter().map(String::from).collect();
        let out = filepath_from_args(v);
        assert!(out.is_ok());
        assert_eq!(String::from("input"), out.unwrap());
    }

    #[test]
    fn list_of_numbers_test_valid_input() {
        let v: Vec<String> = vec!["23", "9", "19"]
            .into_iter()
            .map(String::from)
            .collect();
        let out = list_of_numbers(v);
        assert!(out.is_ok());
        assert_eq!(vec![23, 9, 19], out.unwrap());
    }

    #[test]
    fn list_of_numbers_test_invalid_numbers() {
        let v: Vec<String> = vec!["hello", "World", "19"]
            .into_iter()
            .map(String::from)
            .collect();
        assert!(list_of_numbers(v).is_err());
    }

    #[test]
    fn list_of_numbers_test_empty() {
        let v: Vec<String> = vec![];
        let out = list_of_numbers(v);
        assert!(out.is_ok());
        assert!(out.unwrap().is_empty());
    }
}
