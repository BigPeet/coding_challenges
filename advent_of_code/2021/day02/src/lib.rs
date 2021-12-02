use parsing::InputError;
use std::fmt::Display;
use std::str::FromStr;

pub enum Command {
    Forward(i32),
    Down(i32),
    Up(i32),
}

impl FromStr for Command {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((left, right)) = s.split_once(' ') {
            let value = right.parse::<i32>()?;
            match left {
                "forward" => Ok(Command::Forward(value)),
                "down" => Ok(Command::Down(value)),
                "up" => Ok(Command::Up(value)),
                _ => Err(InputError::ParseGeneral),
            }
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}

pub struct Position {
    pub horizontal: i32,
    pub depth: i32,
}

impl Position {
    pub fn new() -> Position {
        Position {
            horizontal: 0,
            depth: 0,
        }
    }

    pub fn apply(&mut self, cmd: &Command) {
        match cmd {
            Command::Forward(val) => self.horizontal += val,
            Command::Down(val) => self.depth += val,
            Command::Up(val) => self.depth -= val,
        }
    }
}

impl Display for Position {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "({}, {})", self.horizontal, self.depth)
    }
}

pub struct AdvancedPosition {
    pub horizontal: i32,
    pub depth: i32,
    pub aim: i32,
}

impl AdvancedPosition {
    pub fn new() -> AdvancedPosition {
        AdvancedPosition {
            horizontal: 0,
            depth: 0,
            aim: 0,
        }
    }

    pub fn apply(&mut self, cmd: &Command) {
        match cmd {
            Command::Forward(val) => {
                self.horizontal += val;
                self.depth += val * self.aim;
            }
            Command::Down(val) => self.aim += val,
            Command::Up(val) => self.aim -= val,
        }
    }
}

impl Display for AdvancedPosition {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "({}, {})", self.horizontal, self.depth)
    }
}
