use parsing::InputError;
use std::fmt::Display;
use std::str::FromStr;

pub struct OctoGrid {
    octopuses: [[u8; Self::FIELD_SIZE]; Self::FIELD_SIZE],
}

struct Offset {
    x: i32,
    y: i32,
}

#[derive(Clone, Copy, PartialEq, Eq)]
struct GridCoordinate {
    x: usize,
    y: usize,
}

impl GridCoordinate {
    fn add(&self, delta: &Offset) -> Option<GridCoordinate> {
        let x = self.x as i32 + delta.x;
        let y = self.y as i32 + delta.y;

        if x >= 0 && x < OctoGrid::FIELD_SIZE as i32 && y >= 0 && y < OctoGrid::FIELD_SIZE as i32 {
            Some(GridCoordinate {
                x: x as usize,
                y: y as usize,
            })
        } else {
            None
        }
    }
}

struct GridIterator {
    center: GridCoordinate,
    index: usize,
}

impl GridIterator {
    const LENGTH: usize = 8;
    const OFFSETS: [Offset; Self::LENGTH] = [
        Offset { x: -1, y: -1 },
        Offset { x: -1, y: 0 },
        Offset { x: -1, y: 1 },
        Offset { x: 0, y: -1 },
        Offset { x: 0, y: 1 },
        Offset { x: 1, y: -1 },
        Offset { x: 1, y: 0 },
        Offset { x: 1, y: 1 },
    ];
}

impl Iterator for GridIterator {
    type Item = GridCoordinate;

    fn next(&mut self) -> Option<Self::Item> {
        while self.index < Self::LENGTH {
            let d = self.center.add(&Self::OFFSETS[self.index]);
            self.index += 1;
            if d.is_some() {
                return d;
            }
        }
        None
    }
}

impl OctoGrid {
    const FIELD_SIZE: usize = 10;
    const MAX_LEVEL: u8 = 9;

    fn neighbours(&self, coord: GridCoordinate) -> GridIterator {
        GridIterator {
            center: coord,
            index: 0,
        }
    }

    fn increase_energy(&mut self, coord: GridCoordinate, flashed: &mut Vec<GridCoordinate>) {
        self.octopuses[coord.y][coord.x] += 1;
        if self.octopuses[coord.y][coord.x] > Self::MAX_LEVEL && !flashed.contains(&coord) {
            flashed.push(coord);
            for neighbour in self.neighbours(coord) {
                self.increase_energy(neighbour, flashed);
            }
        }
    }

    pub fn step(&mut self) -> u32 {
        let mut flashes = vec![];
        for i in 0..Self::FIELD_SIZE {
            for j in 0..Self::FIELD_SIZE {
                let coord = GridCoordinate { x: j, y: i };
                self.increase_energy(coord, &mut flashes);
            }
        }

        // Reset flashed levels
        for coord in flashes.iter() {
            self.octopuses[coord.y][coord.x] = 0;
        }
        flashes.len() as u32
    }
}

impl Display for OctoGrid {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for row in self.octopuses.iter() {
            for val in row.iter() {
                write!(f, "{}", val)?;
            }
            writeln!(f)?;
        }
        write!(f, "")
    }
}

impl FromStr for OctoGrid {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if s.lines().count() != Self::FIELD_SIZE {
            return Err(InputError::ParseGeneral);
        }

        let mut octopuses = [[0; Self::FIELD_SIZE]; Self::FIELD_SIZE];
        for (row, line) in s.lines().enumerate() {
            for (col, val) in line.chars().enumerate() {
                let energy = val.to_digit(10).ok_or(InputError::ParseGeneral)? as u8;
                octopuses[row][col] = energy;
            }
        }
        Ok(OctoGrid { octopuses })
    }
}
