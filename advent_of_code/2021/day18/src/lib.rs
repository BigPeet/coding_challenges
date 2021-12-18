use parsing::InputError;
use std::fmt::Display;
use std::ops::Add;
use std::str::FromStr;

type NestingInfo = bool;

#[derive(Debug, Clone)]
pub struct SnailNumber {
    numbers: Vec<(u8, Vec<NestingInfo>)>,
}

impl SnailNumber {
    pub fn empty() -> SnailNumber {
        SnailNumber { numbers: vec![] }
    }

    fn find_explosion(&self) -> Option<usize> {
        self.numbers
            .iter()
            .position(|(_, meta_stack)| meta_stack.len() > 4)
    }

    fn find_split(&self) -> Option<usize> {
        self.numbers.iter().position(|&(val, _)| val >= 10)
    }

    fn explode(&mut self) -> bool {
        if let Some(idx) = self.find_explosion() {
            // The first number found will always be the left in the pair.
            // An exploding pair always consist of two numbers.
            let (lval, _) = self.numbers.remove(idx);
            let rval = self.numbers[idx].0;

            // Replace pair with a 0 in the higher pair.
            self.numbers[idx].0 = 0;
            self.numbers[idx].1.pop();

            // Add left exploding value to the next left number.
            if idx > 0 {
                self.numbers[idx - 1].0 += lval;
            }

            // Add right exploding value to the next right number.
            if idx < self.numbers.len() - 1 {
                self.numbers[idx + 1].0 += rval;
            }
            true
        } else {
            false
        }
    }

    fn split(&mut self) -> bool {
        if let Some(idx) = self.find_split() {
            // Calculate new values.
            let val = self.numbers[idx].0;
            let fval = val as f32;
            let lval = (fval / 2.0).floor() as u8;
            let rval = (fval / 2.0).ceil() as u8;

            // Add new right value.
            let mut rmeta = self.numbers[idx].1.clone();
            rmeta.push(false);
            self.numbers.insert(idx + 1, (rval, rmeta));

            // Update left value.
            self.numbers[idx].0 = lval;
            self.numbers[idx].1.push(true);

            true
        } else {
            false
        }
    }

    pub fn magnitude(&self) -> u32 {
        let mut prev_depth = 0;
        let mut stack = vec![];
        let mut res = 0;
        for (val, meta) in self.numbers.iter() {
            let n_depth = meta.len();
            // Add results for all new levels to the stack.
            if n_depth > prev_depth {
                stack.resize(n_depth, 0);
            }
            let mut didx = n_depth;
            let mut intermediate = *val as u32;
            while didx != 0 {
                let stack_res = stack.last_mut().unwrap();
                if meta[didx - 1] {
                    // Store the left result.
                    *stack_res = 3 * intermediate;
                    prev_depth = didx;
                    break;
                } else {
                    // Right result completes the pair.
                    *stack_res += 2 * intermediate;
                    // Propagate upward.
                    intermediate = *stack_res;
                    res = intermediate;
                    stack.pop();
                }
                didx -= 1;
            }
        }
        res
    }

    pub fn is_empty(&self) -> bool {
        self.numbers.is_empty()
    }

    pub fn reduce(&mut self) {
        while self.explode() || self.split() {}
    }
}

impl Add for SnailNumber {
    type Output = Self;

    fn add(mut self, mut rhs: Self) -> Self::Output {
        if self.is_empty() {
            return rhs;
        } else if rhs.is_empty() {
            return self;
        }

        for (_, meta) in self.numbers.iter_mut() {
            meta.insert(0, true);
        }
        for (_, meta) in rhs.numbers.iter_mut() {
            meta.insert(0, false);
        }
        self.numbers.extend(rhs.numbers);
        self.reduce();
        SnailNumber {
            numbers: self.numbers,
        }
    }
}

impl FromStr for SnailNumber {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut meta = vec![];
        let mut numbers = vec![];
        for c in s.trim_end().chars() {
            match c {
                '[' => {
                    meta.push(true);
                }
                ']' => {
                    meta.pop();
                }
                '0'..='9' => {
                    // all input number values are single digit
                    let val = c.to_digit(10).unwrap() as u8;
                    numbers.push((val, meta.clone()));
                }
                ',' => {
                    let l = meta.last_mut().ok_or(InputError::ParseGeneral)?;
                    *l = false;
                }
                _ => return Err(InputError::ParseGeneral),
            }
        }

        Ok(SnailNumber { numbers })
    }
}

impl Display for SnailNumber {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let mut prev_depth = 0;
        for n in self.numbers.iter() {
            let n_depth = n.1.len();

            // Draw preceeding [
            if n_depth > prev_depth {
                for _ in 0..(n_depth - prev_depth) {
                    write!(f, "[")?;
                }
            }

            // Draw number
            write!(f, "{}", n.0)?;

            // Draw trailing , and/or ]
            let mut didx = n_depth;
            while didx != 0 {
                if n.1[didx - 1] {
                    write!(f, ",")?;
                    prev_depth = didx;
                    break;
                } else {
                    write!(f, "]")?;
                }
                didx -= 1;
            }
        }
        write!(f, "")
    }
}
