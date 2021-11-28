use parsing::InputError;
use std::collections::HashMap;
use std::convert::TryFrom;
use std::hash::Hash;

pub trait Coordinate: Clone + Copy + Eq + Hash {
    type NeighbourIter: Iterator + Iterator<Item = Self>;
    fn neighbours(&self) -> Self::NeighbourIter;
    fn new(x: usize, y: usize) -> Self;
}

pub trait Add {
    fn add(&self, other: &Self) -> Self;
}

pub trait SetDimension {
    type ValueType;
    fn set(&mut self, dim: u32, val: Self::ValueType);
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Coordinate3d {
    pub x: i32,
    pub y: i32,
    pub z: i32,
}

impl Coordinate for Coordinate3d {
    type NeighbourIter = NeighbourIterator<Self>;

    fn neighbours(&self) -> Self::NeighbourIter {
        NeighbourIterator::new(*self, 3)
    }

    fn new(x: usize, y: usize) -> Self {
        Coordinate3d {
            x: x as i32,
            y: y as i32,
            z: 0,
        }
    }
}

impl SetDimension for Coordinate3d {
    type ValueType = i32;
    fn set(&mut self, dim: u32, val: Self::ValueType) {
        match dim {
            0 => self.x = val,
            1 => self.y = val,
            2 => self.z = val,
            _ => (),
        }
    }
}

impl Default for Coordinate3d {
    fn default() -> Self {
        Coordinate3d::new(0, 0)
    }
}

impl Add for Coordinate3d {
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

impl Coordinate for Coordinate4d {
    type NeighbourIter = NeighbourIterator<Self>;

    fn neighbours(&self) -> Self::NeighbourIter {
        NeighbourIterator::new(*self, 4)
    }

    fn new(x: usize, y: usize) -> Self {
        Coordinate4d {
            x: x as i32,
            y: y as i32,
            z: 0,
            w: 0,
        }
    }
}

impl SetDimension for Coordinate4d {
    type ValueType = i32;

    fn set(&mut self, dim: u32, val: Self::ValueType) {
        match dim {
            0 => self.x = val,
            1 => self.y = val,
            2 => self.z = val,
            3 => self.w = val,
            _ => (),
        }
    }
}

impl Add for Coordinate4d {
    fn add(&self, other: &Coordinate4d) -> Coordinate4d {
        Coordinate4d {
            x: self.x + other.x,
            y: self.y + other.y,
            z: self.z + other.z,
            w: self.w + other.w,
        }
    }
}

impl Default for Coordinate4d {
    fn default() -> Self {
        Coordinate4d::new(0, 0)
    }
}

pub struct NeighbourIterator<Coord> {
    center: Coord,
    index: usize,
    max: usize,
    pivot: usize,
    deltas: Vec<Coord>,
}

impl<Coord> NeighbourIterator<Coord>
where
    Coord: Default + SetDimension + SetDimension<ValueType = i32>,
{
    pub fn new(center: Coord, dimension: u32) -> NeighbourIterator<Coord> {
        let max = 3u32.pow(dimension) as usize;
        let pivot: u32 = (0..dimension).into_iter().map(|d| 3u32.pow(d)).sum();

        let mut deltas: Vec<Coord> = Vec::with_capacity(max as usize);
        deltas.resize_with(max as usize, Default::default);
        for d in 0..dimension {
            let offset = 3u32.pow(d + 1) as usize;
            let repeat = 3u32.pow(d) as usize;
            for (i, value) in (-1..=1).enumerate() {
                let start = i * repeat;
                for pos in (start..max).step_by(offset) {
                    for repeat_offset in 0..repeat {
                        deltas[pos + repeat_offset].set(d, value);
                    }
                }
            }
        }

        NeighbourIterator {
            center,
            index: 0,
            max,
            pivot: pivot as usize,
            deltas,
        }
    }
}

impl<Coord> Iterator for NeighbourIterator<Coord>
where
    Coord: Default + SetDimension + SetDimension<ValueType = i32> + Add,
{
    type Item = Coord;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index == self.pivot {
            self.index += 1;
        }
        if self.index < self.max {
            let d = &self.deltas[self.index];
            self.index += 1;
            Some(self.center.add(d))
        } else {
            None
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
pub struct CubeMap<Coord> {
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
