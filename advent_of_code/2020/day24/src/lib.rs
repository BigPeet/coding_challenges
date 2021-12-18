use parsing::InputError;
use std::collections::HashSet;
use std::fmt::Display;
use std::str::FromStr;

#[derive(Debug, Clone, Copy)]
pub enum Direction {
    East,
    SouthEast,
    SouthWest,
    West,
    NorthWest,
    NorthEast,
}

#[derive(Default, Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Position {
    x: i32,
    y: i32,
}

impl Position {
    pub fn apply(&mut self, dir: Direction) {
        match dir {
            Direction::East => self.x += 1,
            Direction::SouthEast => self.y -= 1,
            Direction::SouthWest => {
                self.x -= 1;
                self.y -= 1;
            }
            Direction::West => self.x -= 1,
            Direction::NorthWest => self.y += 1,
            Direction::NorthEast => {
                self.x += 1;
                self.y += 1;
            }
        }
    }

    fn neighbours(&self) -> PositionNeighbourIterator {
        PositionNeighbourIterator {
            center: *self,
            index: 0,
        }
    }
}

struct PositionNeighbourIterator {
    center: Position,
    index: usize,
}

impl PositionNeighbourIterator {
    const LEN: usize = 6;
    const OFFSETS: [(i32, i32); Self::LEN] = [(1, 0), (0, -1), (-1, -1), (-1, 0), (0, 1), (1, 1)];
}

impl Iterator for PositionNeighbourIterator {
    type Item = Position;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index < Self::LEN {
            let new_pos = Position {
                x: self.center.x + Self::OFFSETS[self.index].0,
                y: self.center.y + Self::OFFSETS[self.index].1,
            };
            self.index += 1;
            Some(new_pos)
        } else {
            None
        }
    }
}

#[derive(Default)]
pub struct TileMap(HashSet<Position>);

impl TileMap {
    pub fn flip_tile(&mut self, tile_ref: &TileReference) {
        let mut pos = Position::default();
        tile_ref.0.iter().for_each(|&dir| pos.apply(dir));
        if !self.0.remove(&pos) {
            self.0.insert(pos);
        }
    }

    pub fn update(&mut self) {
        let mut relevant_white_tiles = vec![];
        let mut turn_white = vec![];
        let mut turn_black = vec![];
        for pos in self.0.iter() {
            let mut black_count = 0;
            for nb in pos.neighbours() {
                if self.0.contains(&nb) {
                    black_count += 1;
                } else {
                    relevant_white_tiles.push(nb);
                }
            }
            if black_count == 0 || black_count > 2 {
                turn_white.push(*pos);
            }
        }
        for pos in relevant_white_tiles {
            let mut black_count = 0;
            for nb in pos.neighbours() {
                if self.0.contains(&nb) {
                    black_count += 1;
                }
            }
            if black_count == 2 {
                turn_black.push(pos);
            }
        }
        for pos in turn_white {
            self.0.remove(&pos);
        }
        for pos in turn_black {
            self.0.insert(pos);
        }
    }

    pub fn black_tiles(&self) -> usize {
        self.0.len()
    }
}

pub struct TileReference(Vec<Direction>);

impl FromStr for TileReference {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut char_iter = s.trim_end().chars();
        let mut dirs = vec![];
        while let Some(c) = char_iter.next() {
            match c {
                'e' => dirs.push(Direction::East),
                'w' => dirs.push(Direction::West),
                's' => {
                    let next = char_iter.next().ok_or(InputError::ParseGeneral)?;
                    match next {
                        'e' => dirs.push(Direction::SouthEast),
                        'w' => dirs.push(Direction::SouthWest),
                        _ => return Err(InputError::ParseGeneral),
                    }
                }
                'n' => {
                    let next = char_iter.next().ok_or(InputError::ParseGeneral)?;
                    match next {
                        'e' => dirs.push(Direction::NorthEast),
                        'w' => dirs.push(Direction::NorthWest),
                        _ => return Err(InputError::ParseGeneral),
                    }
                }
                _ => return Err(InputError::ParseGeneral),
            }
        }
        Ok(TileReference(dirs))
    }
}

impl Display for TileReference {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for tr in self.0.iter() {
            let out = match tr {
                Direction::East => "e",
                Direction::SouthEast => "se",
                Direction::SouthWest => "sw",
                Direction::West => "w",
                Direction::NorthWest => "nw",
                Direction::NorthEast => "ne",
            };
            write!(f, "{}", out)?;
        }
        write!(f, "")
    }
}
