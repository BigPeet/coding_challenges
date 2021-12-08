use parsing::InputError;
use std::fmt::Display;
use std::str::FromStr;

#[derive(Debug, Clone, Copy)]
pub struct LanternFish {
    timer: u8,
}

impl LanternFish {
    const ESTIMATED_RESET: usize = 7;
    const ESTIMATED_WAITING_TIME: usize = 2;

    pub fn new(starting_age: u8) -> LanternFish {
        LanternFish {
            timer: starting_age,
        }
    }

    pub fn spawn() -> LanternFish {
        LanternFish {
            timer: (Self::ESTIMATED_RESET - 1 + Self::ESTIMATED_WAITING_TIME) as u8,
        }
    }

    pub fn step(&mut self) -> bool {
        if self.timer == 0 {
            self.timer = (Self::ESTIMATED_RESET - 1) as u8;
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

#[derive(Debug, Clone)]
pub struct FishSchool(Vec<LanternFish>);

impl FishSchool {
    const NUM_OF_BINS: usize = LanternFish::ESTIMATED_RESET + LanternFish::ESTIMATED_WAITING_TIME;

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

    pub fn fast_simulation(&self, steps: usize) -> u64 {
        let rem_steps = steps.rem_euclid(LanternFish::ESTIMATED_RESET);
        let skip_steps = steps / LanternFish::ESTIMATED_RESET;

        // Simulate first few steps.
        let mut tmp = self.clone();
        for _ in 0..rem_steps {
            tmp.step();
        }

        // Prepare bins with simulated values.
        let mut bins = [0; Self::NUM_OF_BINS];
        tmp.0.into_iter().for_each(|f| bins[f.timer as usize] += 1);

        // Update bins (see notes.md)
        for _ in 0..skip_steps {
            let old_bins = bins;
            for i in 0..LanternFish::ESTIMATED_RESET {
                let diff_idx = (i as i32 - 2).rem_euclid(Self::NUM_OF_BINS as i32) as usize;
                bins[i] = old_bins[i] + old_bins[diff_idx];
            }
            bins[LanternFish::ESTIMATED_RESET..Self::NUM_OF_BINS].clone_from_slice(
                &old_bins[(LanternFish::ESTIMATED_RESET - 2)..(Self::NUM_OF_BINS - 2)],
            );
        }

        // Return sum of all bins.
        bins.iter().sum()
    }

    pub fn len(&self) -> usize {
        self.0.len()
    }

    pub fn is_empty(&self) -> bool {
        self.len() == 0
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
