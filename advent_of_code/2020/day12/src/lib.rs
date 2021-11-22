use parsing::InputError;
use std::str::FromStr;

#[derive(Debug, Clone)]
pub enum Direction {
    North,
    South,
    East,
    West,
    Forward,
}

pub struct DirectionIterator<'a>(&'a Direction);
impl<'a> Iterator for DirectionIterator<'a> {
    type Item = Direction;

    fn next(&mut self) -> Option<Self::Item> {
        match self.0 {
            Direction::North => self.0 = &Direction::East,
            Direction::South => self.0 = &Direction::West,
            Direction::East => self.0 = &Direction::South,
            Direction::West => self.0 = &Direction::North,
            Direction::Forward => (),
        };
        Some(self.0.clone())
    }
}

impl<'a> DoubleEndedIterator for DirectionIterator<'a> {
    fn next_back(&mut self) -> Option<Self::Item> {
        match self.0 {
            Direction::North => self.0 = &Direction::West,
            Direction::South => self.0 = &Direction::East,
            Direction::East => self.0 = &Direction::North,
            Direction::West => self.0 = &Direction::South,
            Direction::Forward => (),
        };
        Some(self.0.clone())
    }
}

impl Direction {
    pub fn iter(&self) -> DirectionIterator<'_> {
        DirectionIterator(self)
    }
}

#[derive(Debug)]
pub enum RotationDirection {
    Left,
    Right,
}

#[derive(Debug)]
pub enum Instruction {
    Move(Direction, i32),
    Rotate(RotationDirection, i32),
}

impl FromStr for Instruction {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if s.len() > 1 {
            let c = s.chars().nth(0).unwrap();
            let value = s[1..].parse::<i32>()?;
            match c {
                'N' => Ok(Instruction::Move(Direction::North, value)),
                'S' => Ok(Instruction::Move(Direction::South, value)),
                'E' => Ok(Instruction::Move(Direction::East, value)),
                'W' => Ok(Instruction::Move(Direction::West, value)),
                'F' => Ok(Instruction::Move(Direction::Forward, value)),
                'L' => Ok(Instruction::Rotate(RotationDirection::Left, value)),
                'R' => Ok(Instruction::Rotate(RotationDirection::Right, value)),
                _ => Err(InputError::ParseGeneral),
            }
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}

#[derive(Debug)]
pub struct Position {
    x: i32,
    y: i32,
}

impl Position {
    pub fn manhattan_distance(&self) -> usize {
        (self.x.abs() + self.y.abs()) as usize
    }
}

#[derive(Debug)]
pub struct Ferry {
    pos: Position,
    facing: Direction,
}

impl Ferry {
    pub fn new() -> Self {
        Self {
            pos: Position { x: 0, y: 0 },
            facing: Direction::East,
        }
    }

    pub fn position(&self) -> &Position {
        &self.pos
    }

    fn movement(&mut self, direction: &Direction, value: i32) {
        match direction {
            Direction::North => self.pos.y += value,
            Direction::South => self.pos.y -= value,
            Direction::East => self.pos.x += value,
            Direction::West => self.pos.x -= value,
            Direction::Forward => self.movement(&self.facing.clone(), value),
        }
    }

    fn rotate(&mut self, direction: &RotationDirection, degree: i32) {
        let steps = (degree / 90 - 1) as usize;
        self.facing = match direction {
            RotationDirection::Left => self.facing.iter().rev().nth(steps).unwrap(),
            RotationDirection::Right => self.facing.iter().nth(steps).unwrap(),
        };
    }

    pub fn apply(&mut self, inst: &Instruction) {
        match inst {
            Instruction::Move(dir, value) => self.movement(dir, *value),
            Instruction::Rotate(dir, degree) => self.rotate(dir, *degree),
        }
    }
}
