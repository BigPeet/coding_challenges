use parsing::InputError;
use std::collections::HashMap;
use std::collections::HashSet;
use std::str::FromStr;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Position {
    x: i32,
    y: i32,
    z: i32,
}

impl FromStr for Position {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut it = s.split(',');
        let x = it.next().ok_or(InputError::ParseGeneral)?.parse::<i32>()?;
        let y = it.next().ok_or(InputError::ParseGeneral)?.parse::<i32>()?;
        let z = it.next().ok_or(InputError::ParseGeneral)?.parse::<i32>()?;
        Ok(Position { x, y, z })
    }
}

impl Position {
    pub fn distance(&self, other: &Position) -> u32 {
        let dx = self.x - other.x;
        let dy = self.y - other.y;
        let dz = self.z - other.z;
        let d = (dx * dx + dy * dy + dz * dz) as f64;
        d.sqrt() as u32
    }
}

#[derive(Debug)]
pub struct Scanner {
    id: usize,
    beacons: Vec<Position>,
    distances: HashMap<Position, HashSet<u32>>,
    // translation + rotation info
}

impl Scanner {
    fn calculate_distances(beacons: &[Position]) -> HashMap<Position, HashSet<u32>> {
        let mut distances = HashMap::new();
        for b1 in beacons.iter() {
            let e = distances.entry(*b1).or_insert(HashSet::new());
            for b2 in beacons.iter() {
                if b1 != b2 {
                    e.insert(b1.distance(b2));
                }
            }
        }
        distances
    }

    pub fn new(id: usize, beacons: Vec<Position>) -> Scanner {
        let distances = Self::calculate_distances(&beacons);
        Scanner {
            id,
            beacons,
            distances,
        }
    }

    pub fn max_common_distances(&self, other: &Scanner) -> usize {
        let mut maximum = 0;
        for b1 in self.beacons.iter() {
            let d1 = &self.distances[b1];
            for b2 in other.beacons.iter() {
                let d2 = &other.distances[b2];
                let common_dists = d1.intersection(d2).count();
                if common_dists > maximum {
                    maximum = common_dists;
                }
            }
        }
        maximum
    }
}
