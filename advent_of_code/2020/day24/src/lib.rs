use parsing::InputError;
use std::collections::HashMap;
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
}

#[derive(Default)]
pub struct TileMap(HashMap<Position, bool>);

impl TileMap {
    pub fn flip_tile(&mut self, tile_ref: &TileReference) {
        let mut pos = Position::default();
        tile_ref.0.iter().for_each(|&dir| pos.apply(dir));
        self.0
            .entry(pos)
            .and_modify(|flipped| *flipped = !*flipped)
            .or_insert(true);
    }

    pub fn black_tiles(&self) -> Vec<Position> {
        self.0
            .iter()
            .filter_map(|(&pos, &flipped)| if flipped { Some(pos) } else { None })
            .collect()
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
