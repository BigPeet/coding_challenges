use parsing::InputError;
use std::fmt::Display;
use std::str::FromStr;

#[derive(Debug)]
pub struct LanternFish {
    timer: u8,
}

impl LanternFish {
    const ESTIMATED_RESET: u8 = 6;
    const ESTIMATED_WAITING_TIME: u8 = 2;

    pub fn new(starting_age: u8) -> LanternFish {
        LanternFish {
            timer: starting_age,
        }
    }

    pub fn spawn() -> LanternFish {
        LanternFish {
            timer: Self::ESTIMATED_RESET + Self::ESTIMATED_WAITING_TIME,
        }
    }

    pub fn step(&mut self) -> bool {
        if self.timer == 0 {
            self.timer = Self::ESTIMATED_RESET;
            true
        } else {
            self.timer -= 1;
            false
        }
    }
}

impl Display for LanternFish {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.timer)
    }
}

#[derive(Debug)]
pub struct FishSchool(Vec<LanternFish>);

impl FishSchool {
    pub fn step(&mut self) {
        let mut new_fish = 0;
        for fish in self.0.iter_mut() {
            if fish.step() {
                new_fish += 1;
            }
        }
        for _ in 0..new_fish {
            self.0.push(LanternFish::spawn());
        }
    }

    pub fn simulate(&mut self, steps: usize) {
        for _ in 0..steps {
            self.step();
        }
    }

    pub fn len(&self) -> usize {
        self.0.len()
    }
}

impl Display for FishSchool {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for fish in self.0.iter() {
            write!(f, "{} ", fish)?;
        }
        write!(f, "")
    }
}

impl FromStr for FishSchool {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut fishes = vec![];
        for tok in s.trim().split(',') {
            let val = tok.parse::<u8>()?;
            fishes.push(LanternFish::new(val));
        }
        Ok(FishSchool(fishes))
    }
}
