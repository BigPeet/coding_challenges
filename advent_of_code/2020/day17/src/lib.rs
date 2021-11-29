use parsing::InputError;
use std::collections::HashMap;
use std::convert::TryFrom;
use std::hash::Hash;

#[derive(Debug, Clone, Copy, Eq)]
pub struct Coordinate<const DIMENSION: usize> {
    data: [i32; DIMENSION],
}

impl<const DIMENSION: usize> PartialEq for Coordinate<DIMENSION> {
    fn eq(&self, other: &Self) -> bool {
        self.data == other.data
    }
}

impl<const DIMENSION: usize> Hash for Coordinate<DIMENSION> {
    fn hash<H: std::hash::Hasher>(&self, state: &mut H) {
        self.data.hash(state)
    }
}

impl<const DIMENSION: usize> Default for Coordinate<DIMENSION> {
    fn default() -> Self {
        Coordinate {
            data: [0; DIMENSION],
        }
    }
}

impl<const DIMENSION: usize> Coordinate<DIMENSION> {
    fn neighbours(&self) -> NeighbourIterator<DIMENSION> {
        NeighbourIterator::new(self)
    }

    fn new(x: i32, y: i32) -> Self {
        let mut data = [0i32; DIMENSION];
        if DIMENSION > 0 {
            data[0] = x;
        }
        if DIMENSION > 1 {
            data[1] = y;
        }
        Coordinate { data }
    }

    fn add(&self, other: &Self) -> Self {
        let mut data = [0; DIMENSION];
        for (i, val) in data.iter_mut().enumerate().take(DIMENSION) {
            *val = self.data[i] + other.data[i];
        }
        Coordinate { data }
    }

    fn set(&mut self, dim: u32, val: i32) {
        self.data[dim as usize] = val;
    }
}

pub struct NeighbourIterator<'a, const DIMENSION: usize> {
    center: &'a Coordinate<DIMENSION>,
    index: usize,
    max: usize,
    pivot: usize,
    deltas: Vec<Coordinate<DIMENSION>>, // would love to make this [Coordinate<DIMENSION>; 3u32.pow(DIMENSION)]
}

impl<'a, const DIMENSION: usize> NeighbourIterator<'a, DIMENSION> {

    // would love to make this "static"
    fn create_neighbours() -> Vec<Coordinate<DIMENSION>> {
        let dimension: u32 = DIMENSION as u32;
        let max = 3u32.pow(dimension) as usize;
        let mut deltas = vec![Coordinate::default(); max];
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
        deltas
    }

    fn new(center: &'a Coordinate<DIMENSION>) -> NeighbourIterator<DIMENSION> {
        let dimension: u32 = DIMENSION as u32;
        let max = 3u32.pow(dimension) as usize;
        let pivot: u32 = (0..dimension).into_iter().map(|d| 3u32.pow(d)).sum();
        let deltas = Self::create_neighbours();
        NeighbourIterator {
            center,
            index: 0,
            max,
            pivot: pivot as usize,
            deltas,
        }
    }
}

impl<'a, const DIMENSION: usize> Iterator for NeighbourIterator<'a, DIMENSION> {
    type Item = Coordinate<DIMENSION>;

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
pub struct CubeMap<const DIMENSION: usize> {
    hmap: HashMap<Coordinate<DIMENSION>, CellStatus>,
}

impl<const DIMENSION: usize> CubeMap<DIMENSION> {
    pub fn new(lines: &[String]) -> Result<Self, InputError> {
        let mut cmap = CubeMap {
            hmap: HashMap::new(),
        };
        for (row, line) in lines.iter().enumerate() {
            for (col, c) in line.trim().chars().enumerate() {
                let status = CellStatus::try_from(c)?;
                let coord = Coordinate::new(col as i32, row as i32);
                cmap.insert(coord, status);
            }
        }
        Ok(cmap)
    }

    fn insert(&mut self, coord: Coordinate<DIMENSION>, status: CellStatus) {
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
