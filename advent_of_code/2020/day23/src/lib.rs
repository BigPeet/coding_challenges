use parsing::InputError;
use std::collections::LinkedList;
use std::fmt::Display;
use std::str::FromStr;

type Cup = usize;

pub struct CupsGame {
    // since the game will require splitting and merging the cups at arbitrary indeces, this seems
    // to be a case, where a LinkedList might be more practical than a Vec.
    // But a Vec or VecDeque might still be better...
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
            cups.push_back(c.to_digit(10).ok_or(InputError::ParseGeneral)? as Cup);
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

pub struct RealCupsGame {
    cups: Vec<Cup>,
    current: Cup,
    min: Cup,
}

impl FromStr for RealCupsGame {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut all_cups = Vec::with_capacity(Self::MAX_VALUE);
        // Read in with provided cups.
        for c in s.trim_end().chars() {
            all_cups.push(c.to_digit(10).ok_or(InputError::ParseGeneral)? as Cup);
        }

        // Fill up to 1 million.
        let max = *all_cups.iter().max().unwrap_or(&0);
        let min = *all_cups.iter().min().unwrap_or(&0);
        for cup in (max + 1)..=Self::MAX_VALUE as Cup {
            all_cups.push(cup);
        }

        // Cups go from 1 -> 1 million
        // So 1 million needs to be a valid index.
        let mut cups = vec![0; Self::MAX_VALUE + 1];

        // Build specialized next-cup-map.
        for i in 0..Self::MAX_VALUE - 1 {
            cups[all_cups[i]] = all_cups[i + 1];
        }
        cups[all_cups[Self::MAX_VALUE - 1]] = all_cups[0];
        Ok(RealCupsGame {
            cups,
            current: all_cups[0],
            min,
        })
    }
}

impl RealCupsGame {
    const MAX_VALUE: usize = 1_000_000;
    pub fn play_move(&mut self) {
        let next1 = self.cups[self.current];
        let next2 = self.cups[next1];
        let next3 = self.cups[next2];

        // Find destination
        let mut destination_cup = self.current - 1;
        while destination_cup < self.min
            || destination_cup == next1
            || destination_cup == next2
            || destination_cup == next3
        {
            if destination_cup < self.min {
                destination_cup = Self::MAX_VALUE as Cup;
            } else {
                destination_cup -= 1;
            }
        }
        self.cups[self.current] = self.cups[next3];
        self.cups[next3] = self.cups[destination_cup];
        self.cups[destination_cup] = next1;

        self.current = self.cups[self.current];
    }

    pub fn score(&self) -> u64 {
        let first = self.cups[1];
        let second = self.cups[first];
        first as u64 * second as u64
    }
}
