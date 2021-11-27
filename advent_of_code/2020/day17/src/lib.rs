use parsing::InputError;
use std::collections::HashMap;
use std::convert::TryFrom;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Coordinate {
    pub x: i32,
    pub y: i32,
    pub z: i32,
}

struct CoordinateNeighbourIter {
    center: Coordinate,
    index: usize,
    deltas: [Coordinate; 26],
}

impl CoordinateNeighbourIter {
    pub fn new(center: Coordinate) -> CoordinateNeighbourIter {
        CoordinateNeighbourIter {
            center,
            index: 0,
            deltas: CoordinateNeighbourIter::deltas(),
        }
    }
    fn deltas() -> [Coordinate; 26] {
        let mut d: [Coordinate; 26] = [Coordinate { x: 0, y: 0, z: 0 }; 26];
        let mut idx = 0;
        for x in -1..=1 {
            for y in -1..=1 {
                for z in -1..=1 {
                    if x != 0 || y != 0 || z != 0 {
                        d[idx] = Coordinate { x, y, z };
                        idx += 1;
                    }
                }
            }
        }
        d
    }
}

impl Iterator for CoordinateNeighbourIter {
    type Item = Coordinate;

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

impl Coordinate {
    fn neighbours(&self) -> CoordinateNeighbourIter {
        CoordinateNeighbourIter::new(*self)
    }

    fn add(&self, other: &Coordinate) -> Coordinate {
        Coordinate {
            x: self.x + other.x,
            y: self.y + other.y,
            z: self.z + other.z,
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
pub struct CubeMap {
    hmap: HashMap<Coordinate, CellStatus>,
}

impl CubeMap {
    pub fn new(lines: &[String]) -> Result<Self, InputError> {
        let mut cmap = CubeMap {
            hmap: HashMap::new(),
        };
        for (row, line) in lines.iter().enumerate() {
            for (col, c) in line.trim().chars().enumerate() {
                let status = CellStatus::try_from(c)?;
                let coord = Coordinate {
                    x: col as i32,
                    y: row as i32,
                    z: 0,
                };
                cmap.insert(coord, status);
            }
        }
        Ok(cmap)
    }

    pub fn insert(&mut self, coord: Coordinate, status: CellStatus) {
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
