use parsing::InputError;
use std::collections::HashMap;
use std::collections::HashSet;
use std::str::FromStr;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Vec3 {
    x: i32,
    y: i32,
    z: i32,
}

impl FromStr for Vec3 {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut it = s.split(',');
        let x = it.next().ok_or(InputError::ParseGeneral)?.parse::<i32>()?;
        let y = it.next().ok_or(InputError::ParseGeneral)?.parse::<i32>()?;
        let z = it.next().ok_or(InputError::ParseGeneral)?.parse::<i32>()?;
        Ok(Vec3 { x, y, z })
    }
}

impl Vec3 {
    pub fn null() -> Vec3 {
        Vec3 { x: 0, y: 0, z: 0 }
    }
    pub fn distance(&self, other: &Vec3) -> Vec3 {
        Vec3 {
            x: self.x - other.x,
            y: self.y - other.y,
            z: self.z - other.z,
        }
    }
    pub fn comparable_distance(&self, other: &Vec3) -> Vec3 {
        let mut d = self.distance(other);
        d.x = d.x.abs();
        d.y = d.y.abs();
        d.z = d.z.abs();
        let min = d.x.min(d.y).min(d.z);
        let max = d.x.max(d.y).max(d.z);
        let mid = d.x + d.y + d.z - min - max;
        Vec3 {
            x: max,
            y: mid,
            z: min,
        }
    }

    pub fn translate(&self, other: &Vec3) -> Vec3 {
        Vec3 {
            x: self.x + other.x,
            y: self.y + other.y,
            z: self.z + other.z,
        }
    }
}

#[derive(Copy, Clone, Debug)]
enum Coordinate {
    X,
    Y,
    Z,
}

impl Coordinate {
    fn apply(&self, v: &Vec3) -> i32 {
        match self {
            Coordinate::X => v.x,
            Coordinate::Y => v.y,
            Coordinate::Z => v.z,
        }
    }
}

#[derive(Copy, Clone, Debug)]
enum Sign {
    P(Coordinate),
    N(Coordinate),
}

impl Sign {
    fn apply(&self, v: &Vec3) -> i32 {
        match self {
            Sign::P(c) => c.apply(v),
            Sign::N(c) => -c.apply(v),
        }
    }
}

#[derive(Copy, Clone, Debug)]
struct Rotation(Sign, Sign, Sign);

impl Rotation {
    fn apply(&self, v: &Vec3) -> Vec3 {
        Vec3 {
            x: self.0.apply(v),
            y: self.1.apply(v),
            z: self.2.apply(v),
        }
    }

    fn rotations() -> impl Iterator<Item = Self> {
        let px = Sign::P(Coordinate::X);
        let nx = Sign::N(Coordinate::X);
        let py = Sign::P(Coordinate::Y);
        let ny = Sign::N(Coordinate::Y);
        let pz = Sign::P(Coordinate::Z);
        let nz = Sign::N(Coordinate::Z);

        [
            Rotation(px, py, pz),
            Rotation(px, py, nz),
            Rotation(px, ny, pz),
            Rotation(px, ny, nz),
            Rotation(nx, py, pz),
            Rotation(nx, py, nz),
            Rotation(nx, ny, pz),
            Rotation(nx, ny, nz),
        ]
        .into_iter()
        .flat_map(|signed| {
            let Rotation(x, y, z) = signed;
            [
                Rotation(x, y, z),
                Rotation(x, z, y),
                Rotation(y, x, z),
                Rotation(y, z, x),
                Rotation(z, x, y),
                Rotation(z, y, x),
            ]
            .into_iter()
        })
    }
}

#[derive(Copy, Clone, Debug)]
pub struct Transformation(Rotation, Vec3);

impl Transformation {
    fn apply(&self, v: &Vec3) -> Vec3 {
        self.0.apply(v).translate(&self.1)
    }
}

#[derive(Debug, Eq)]
pub struct Scanner {
    id: usize,
    beacons: Vec<Vec3>,
    distances: HashMap<Vec3, HashSet<Vec3>>,
    // translation + rotation info
}

impl PartialEq for Scanner {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

impl Scanner {
    const NEEDED_CONNECTIONS: usize = 11;

    fn calculate_distances(beacons: &[Vec3]) -> HashMap<Vec3, HashSet<Vec3>> {
        let mut distances = HashMap::new();
        for b1 in beacons.iter() {
            let e = distances.entry(*b1).or_insert(HashSet::new());
            for b2 in beacons.iter() {
                if b1 != b2 {
                    e.insert(b1.comparable_distance(b2));
                }
            }
        }
        distances
    }

    pub fn new(id: usize, beacons: Vec<Vec3>) -> Scanner {
        let distances = Self::calculate_distances(&beacons);
        Scanner {
            id,
            beacons,
            distances,
        }
    }

    pub fn id(&self) -> usize {
        self.id
    }

    pub fn beacons(&self) -> usize {
        self.beacons.len()
    }

    pub fn overlaps(&self, other: &Scanner) -> bool {
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
        maximum >= Self::NEEDED_CONNECTIONS
    }

    pub fn try_transformation(&self, other: &Scanner) -> Option<Transformation> {
        // find a beacon which is shared by both scanners
        for b1 in self.beacons.iter() {
            let d1 = &self.distances[b1];
            for b2 in other.beacons.iter() {
                let d2 = &other.distances[b2];
                if d1.intersection(d2).count() >= Self::NEEDED_CONNECTIONS {
                    // Ok, shared beacon found.
                    // Find rotation / translation
                    let b1_dists = self
                        .beacons
                        .iter()
                        .map(|b| b.distance(b1))
                        .filter(|&d| d != Vec3::null())
                        .collect::<HashSet<_>>();
                    for rot in Rotation::rotations() {
                        let rot_dists = other
                            .beacons
                            .iter()
                            .map(|x| rot.apply(&x.distance(b2)))
                            .filter(|&d| d != Vec3::null())
                            .collect::<HashSet<_>>();
                        if b1_dists.intersection(&rot_dists).count() >= Self::NEEDED_CONNECTIONS {
                            // Return rotation and translation.
                            return Some(Transformation(rot, b1.distance(&rot.apply(b2))));
                        }
                    }
                }
            }
        }
        None
    }

    pub fn combine(&mut self, mut other: Scanner, t: Transformation) {
        for ob in other.beacons.iter() {
            let transformed = t.apply(ob);
            if !self.beacons.contains(&transformed) {
                self.beacons.push(transformed);
            }
            if let Some(ds) = other.distances.remove(ob) {
                self.distances.entry(transformed).or_default().extend(ds)
            }
        }
    }
}
