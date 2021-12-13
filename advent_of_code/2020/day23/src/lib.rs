use parsing::InputError;
use std::collections::LinkedList;
use std::fmt::Display;
use std::str::FromStr;

type Cup = u32;

pub struct CupsGame {
    // since the game will require splitting and merging the cups at arbitrary indeces, this seems
    // to be a case, where a LinkedList might be more practical than a Vec.
    // But a Vec might still be better...
    cups: LinkedList<Cup>,
    min: Cup,
    max: Cup,
}

impl CupsGame {
    pub fn play_move(&mut self) {
        let head = self.cups.pop_front().unwrap();
        let mut tail = self.cups.split_off(3);

        // find destination cup
        let mut candidate = head;
        let mut destination = None;
        while destination.is_none() {
            if candidate == self.min {
                candidate = self.max;
            } else {
                candidate -= 1;
            }
            if !self.cups.contains(&candidate) {
                destination = Some(candidate);
            }
        }
        let destination = destination.unwrap();
        let pos = tail.iter().position(|cup| *cup == destination).unwrap();
        let mut right_tail = tail.split_off(pos + 1);
        tail.append(&mut self.cups);
        tail.append(&mut right_tail);
        tail.push_back(head);
        self.cups = tail;
    }

    pub fn order_by(&mut self, cup: Cup) {
        if let Some(pos) = self.cups.iter().position(|elem| *elem == cup) {
            if pos > 0 && pos < self.cups.len() {
                let mut tail = self.cups.split_off(pos);
                tail.append(&mut self.cups);
                self.cups = tail;
            }
        }
    }

    pub fn score(&mut self) -> String {
        self.order_by(1);
        // FIXME Horrible
        format!("{}", self)[1..].to_string()
    }
}

impl FromStr for CupsGame {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if s.is_empty() {
            return Err(InputError::ParseGeneral);
        }
        let mut cups = LinkedList::new();
        for c in s.trim_end().chars() {
            cups.push_back(c.to_digit(10).ok_or(InputError::ParseGeneral)?);
        }
        let min = *cups.iter().min().unwrap_or(&0);
        let max = *cups.iter().max().unwrap_or(&0);
        Ok(CupsGame { cups, min, max })
    }
}

impl Display for CupsGame {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for cup in self.cups.iter() {
            write!(f, "{}", cup)?;
        }
        write!(f, "")
    }
}
