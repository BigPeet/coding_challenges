use parsing::InputError;
use std::cmp::Ordering;
use std::str::FromStr;

#[derive(Debug)]
pub struct Policy {
    first: i32,
    second: i32,
    letter: char,
}

impl FromStr for Policy {
    type Err = InputError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((min, suffix)) = s.split_once("-") {
            if let Some((max, letter)) = suffix.split_once(" ") {
                return Ok(Policy {
                    first: min.parse::<i32>()?,
                    second: max.parse::<i32>()?,
                    letter: letter.parse::<char>()?,
                });
            }
        }
        Err(InputError::ParseGeneral)
    }
}

impl Policy {
    pub fn validate_p1(&self, password: &str) -> bool {
        let occurrences: i32 = password
            .chars()
            .filter(|c| c.cmp(&self.letter) == Ordering::Equal)
            .count() as i32;

        occurrences >= self.first && occurrences <= self.second
    }
}
