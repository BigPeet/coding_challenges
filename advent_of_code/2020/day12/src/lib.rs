use parsing::InputError;
use std::mem;
use std::str::FromStr;

#[derive(Debug, Copy, Clone)]
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
        Some(*self.0)
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
        Some(*self.0)
    }
}

impl Direction {
    pub fn iter(&self) -> DirectionIterator<'_> {
        DirectionIterator(self)
    }
}

#[derive(Debug, Copy, Clone)]
pub enum RotationDirection {
    Left,
    Right,
}

#[derive(Debug, Copy, Clone)]
pub enum Instruction {
    Move(Direction, i32),
    Rotate(RotationDirection, i32),
}

impl FromStr for Instruction {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if s.len() > 1 {
            let c = s.chars().next().unwrap();
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

#[derive(Debug, Copy, Clone)]
pub struct Position {
    x: i32,
    y: i32,
}

impl Position {
    pub fn manhattan_distance(&self) -> usize {
        (self.x.abs() + self.y.abs()) as usize
    }

    pub fn translate(&mut self, direction: &Direction, value: i32) {
        match direction {
            Direction::North => self.y += value,
            Direction::South => self.y -= value,
            Direction::East => self.x += value,
            Direction::West => self.x -= value,
            _ => (),
        }
    }

    fn rotate_right(&mut self) {
        mem::swap(&mut self.x, &mut self.y);
        self.y *= -1;
    }

    pub fn rotate(&mut self, direction: &RotationDirection, degree: i32) {
        let mut steps: u8 = (degree / 90 % 4) as u8;
        if matches!(direction, RotationDirection::Left) {
            // treat everything as a rotation the right
            if steps & 0b01 == 0b01 {
                // steps is either 1 or 3 => flip the 2nd bit
                steps ^= 0b10;
            }
        }
        for _ in 0..steps {
            self.rotate_right()
        }
    }
}

pub trait Navigable {
    fn apply(&mut self, inst: &Instruction) {
        match inst {
            Instruction::Move(dir, value) => self.movement(dir, *value),
            Instruction::Rotate(dir, degree) => self.rotate(dir, *degree),
        }
    }

    fn movement(&mut self, direction: &Direction, value: i32);
    fn rotate(&mut self, direction: &RotationDirection, degree: i32);

    fn position(&self) -> Position;
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
}

impl Navigable for Ferry {
    fn movement(&mut self, direction: &Direction, value: i32) {
        match direction {
            Direction::Forward => self.pos.translate(&self.facing, value),
            _ => self.pos.translate(direction, value),
        }
    }

    fn rotate(&mut self, direction: &RotationDirection, degree: i32) {
        let steps = (degree / 90) as usize;
        self.facing = match direction {
            RotationDirection::Left => self.facing.iter().rev().nth(steps - 1).unwrap(),
            RotationDirection::Right => self.facing.iter().nth(steps - 1).unwrap(),
        };
    }

    fn position(&self) -> Position {
        self.pos
    }
}

impl Default for Ferry {
    fn default() -> Self {
        Self::new()
    }
}

#[derive(Debug)]
pub struct WaypointFerry {
    pos: Position,
    waypoint: Position,
}

impl WaypointFerry {
    pub fn new() -> Self {
        Self {
            pos: Position { x: 0, y: 0 },
            waypoint: Position { x: 10, y: 1 },
        }
    }
}

impl Default for WaypointFerry {
    fn default() -> Self {
        Self::new()
    }
}

impl Navigable for WaypointFerry {
    fn movement(&mut self, direction: &Direction, value: i32) {
        match direction {
            Direction::Forward => {
                self.pos.x += value * self.waypoint.x;
                self.pos.y += value * self.waypoint.y;
            }
            _ => self.waypoint.translate(direction, value),
        }
    }

    fn rotate(&mut self, direction: &RotationDirection, degree: i32) {
        self.waypoint.rotate(direction, degree);
    }

    fn position(&self) -> Position {
        self.pos
    }
}
