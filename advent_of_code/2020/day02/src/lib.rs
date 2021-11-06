use parsing::InputError;
use std::cmp::Ordering;
use std::str::FromStr;

#[derive(Debug)]
struct PolicyData {
    first: usize,
    second: usize,
    letter: char,
}

impl FromStr for PolicyData {
    type Err = InputError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((min, suffix)) = s.split_once("-") {
            if let Some((max, letter)) = suffix.split_once(" ") {
                return Ok(PolicyData {
                    first: min.parse::<usize>()?,
                    second: max.parse::<usize>()?,
                    letter: letter.parse::<char>()?,
                });
            }
        }
        Err(InputError::ParseGeneral)
    }
}

pub struct OldPolicy(PolicyData);
pub struct NewPolicy(PolicyData);

pub trait ValidatePassword {
    fn validate(&self, password: &str) -> bool;
}

impl FromStr for OldPolicy {
    type Err = InputError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        Ok(OldPolicy(s.parse::<PolicyData>()?))
    }
}

impl ValidatePassword for OldPolicy {
    fn validate(&self, password: &str) -> bool {
        let occurrences = password
            .chars()
            .filter(|c| c.cmp(&self.0.letter) == Ordering::Equal)
            .count();

        occurrences >= self.0.first && occurrences <= self.0.second
    }
}

impl FromStr for NewPolicy {
    type Err = InputError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        Ok(NewPolicy(s.parse::<PolicyData>()?))
    }
}

impl ValidatePassword for NewPolicy {
    fn validate(&self, password: &str) -> bool {
        let mut occurrences = 0;
        for (idx, c) in password.char_indices() {
            if (idx == self.0.first || idx == self.0.second) && c == self.0.letter {
                occurrences += 1;
            }
        }
        occurrences == 1
    }
}

pub fn count_valid_passwords<Policy: ValidatePassword + FromStr>(lines: &[String]) -> usize {
    // FIXME: report errors when parsing policy
    let valid: Vec<bool> = lines
        .iter()
        .filter_map(|line| {
            line.split_once(":")
                .and_then(|(l, r)| l.parse::<Policy>().map(|pol| pol.validate(r)).ok())
        })
        .collect();
    valid.iter().filter(|v| **v).count()
}
