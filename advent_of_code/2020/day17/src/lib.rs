use parsing::InputError;
use std::collections::HashMap;
use std::convert::TryFrom;
use std::hash::Hash;

pub trait Coordinate: Clone + Copy + Eq + Hash {
    type NeighbourIter: Iterator + Iterator<Item = Self>;
    fn neighbours(&self) -> Self::NeighbourIter;
    fn new(x: usize, y: usize) -> Self;
    fn add(&self, other: &Self) -> Self;
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Coordinate3d {
    pub x: i32,
    pub y: i32,
    pub z: i32,
}

pub struct Neighbour3dIterator {
    center: Coordinate3d,
    index: usize,
    deltas: [Coordinate3d; 26],
}

impl Neighbour3dIterator {
    pub fn new(center: Coordinate3d) -> Neighbour3dIterator {
        Neighbour3dIterator {
            center,
            index: 0,
            deltas: Neighbour3dIterator::deltas(),
        }
    }
    fn deltas() -> [Coordinate3d; 26] {
        let mut d: [Coordinate3d; 26] = [Coordinate3d { x: 0, y: 0, z: 0 }; 26];
        let mut idx = 0;
        for x in -1..=1 {
            for y in -1..=1 {
                for z in -1..=1 {
                    if x != 0 || y != 0 || z != 0 {
                        d[idx] = Coordinate3d { x, y, z };
                        idx += 1;
                    }
                }
            }
        }
        d
    }
}

impl Iterator for Neighbour3dIterator {
    type Item = Coordinate3d;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index < self.deltas.len() {
            let d = &self.deltas[self.index];
            self.index += 1;
            Some(self.center.add(d))
        } else {
            None
        }
    }
}

impl Coordinate for Coordinate3d {
    type NeighbourIter = Neighbour3dIterator;

    fn neighbours(&self) -> Self::NeighbourIter {
        Neighbour3dIterator::new(*self)
    }

    fn new(x: usize, y: usize) -> Self {
        Coordinate3d {
            x: x as i32,
            y: y as i32,
            z: 0,
        }
    }

    fn add(&self, other: &Coordinate3d) -> Coordinate3d {
        Coordinate3d {
            x: self.x + other.x,
            y: self.y + other.y,
            z: self.z + other.z,
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Coordinate4d {
    pub x: i32,
    pub y: i32,
    pub z: i32,
    pub w: i32,
}

pub struct Neighbour4dIterator {
    center: Coordinate4d,
    index: usize,
    deltas: [Coordinate4d; 80],
}

impl Neighbour4dIterator {
    pub fn new(center: Coordinate4d) -> Neighbour4dIterator {
        Neighbour4dIterator {
            center,
            index: 0,
            deltas: Neighbour4dIterator::deltas(),
        }
    }
    fn deltas() -> [Coordinate4d; 80] {
        let mut d: [Coordinate4d; 80] = [Coordinate4d {
            x: 0,
            y: 0,
            z: 0,
            w: 0,
        }; 80];
        let mut idx = 0;
        for x in -1..=1 {
            for y in -1..=1 {
                for z in -1..=1 {
                    for w in -1..=1 {
                        if x != 0 || y != 0 || z != 0 || w != 0 {
                            d[idx] = Coordinate4d { x, y, z, w };
                            idx += 1;
                        }
                    }
                }
            }
        }
        d
    }
}

impl Iterator for Neighbour4dIterator {
    type Item = Coordinate4d;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index < self.deltas.len() {
            let d = &self.deltas[self.index];
            self.index += 1;
            Some(self.center.add(d))
        } else {
            None
        }
    }
}

impl Coordinate for Coordinate4d {
    type NeighbourIter = Neighbour4dIterator;

    fn neighbours(&self) -> Self::NeighbourIter {
        Neighbour4dIterator::new(*self)
    }

    fn new(x: usize, y: usize) -> Self {
        Coordinate4d {
            x: x as i32,
            y: y as i32,
            z: 0,
            w: 0,
        }
    }

    fn add(&self, other: &Coordinate4d) -> Coordinate4d {
        Coordinate4d {
            x: self.x + other.x,
            y: self.y + other.y,
            z: self.z + other.z,
            w: self.w + other.w,
        }
    }
}


#[derive(Debug, Clone, Copy)]
pub enum CellStatus {
    Active,
    Inactive,
}

impl TryFrom<char> for CellStatus {
    type Error = InputError;
    fn try_from(value: char) -> Result<Self, Self::Error> {
        match value {
            '#' => Ok(CellStatus::Active),
            '.' => Ok(CellStatus::Inactive),
            _ => Err(InputError::ParseGeneral),
        }
    }
}

#[derive(Debug)]
pub struct CubeMap<Coord: Coordinate> {
    hmap: HashMap<Coord, CellStatus>,
}

impl<Coord: Coordinate> CubeMap<Coord> {
    pub fn new(lines: &[String]) -> Result<Self, InputError> {
        let mut cmap = CubeMap {
            hmap: HashMap::new(),
        };
        for (row, line) in lines.iter().enumerate() {
            for (col, c) in line.trim().chars().enumerate() {
                let status = CellStatus::try_from(c)?;
                let coord = Coord::new(col, row);
                cmap.insert(coord, status);
            }
        }
        Ok(cmap)
    }

    pub fn insert(&mut self, coord: Coord, status: CellStatus) {
        self.hmap.insert(coord, status);
        if matches!(status, CellStatus::Active) {
            for ncoord in coord.neighbours() {
                self.hmap.entry(ncoord).or_insert(CellStatus::Inactive);
            }
        }
    }

    pub fn cycle(&mut self) {
        let mut updates = Vec::new();
        for coord in self.hmap.keys() {
            let alive_neighbours = coord
                .neighbours()
                .filter_map(|ncoord| self.hmap.get(&ncoord))
                .filter(|status| matches!(status, CellStatus::Active))
                .count();
            let status = self.hmap.get(coord).unwrap();
            match status {
                CellStatus::Active if alive_neighbours != 2 && alive_neighbours != 3 => {
                    updates.push((*coord, CellStatus::Inactive));
                }
                CellStatus::Inactive if alive_neighbours == 3 => {
                    updates.push((*coord, CellStatus::Active));
                }
                _ => (),
            }
        }
        for (coord, status) in updates.into_iter() {
            self.insert(coord, status);
        }
    }

    pub fn alive_cells(&self) -> usize {
        self.hmap
            .values()
            .filter(|status| matches!(status, CellStatus::Active))
            .count()
    }
}
