use parsing::InputError;
use std::collections::HashMap;
use std::str::FromStr;

#[derive(Debug, Copy, Clone)]
pub enum State {
    On,
    Off,
}

impl FromStr for State {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        match s {
            "on" => Ok(State::On),
            "off" => Ok(State::Off),
            _ => Err(InputError::ParseGeneral),
        }
    }
}

#[derive(Debug)]
struct MyInclusiveRange {
    start: i32,
    end: i32,
}

impl FromStr for MyInclusiveRange {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((start, end)) = s[2..].split_once("..") {
            let s = start.parse::<i32>()?;
            let e = end.parse::<i32>()?;
            Ok(MyInclusiveRange { start: s, end: e })
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}

impl MyInclusiveRange {
    fn overlap(&self, other: &Self) -> Option<Self> {
        if self.end < other.start || self.start > other.end {
            // There is no overlap.
            None
        } else {
            let start = self.start.max(other.start);
            let end = self.end.min(other.end);
            Some(MyInclusiveRange { start, end })
        }
    }
}

#[derive(Debug)]
pub struct Cuboid {
    xr: MyInclusiveRange,
    yr: MyInclusiveRange,
    zr: MyInclusiveRange,
}

impl FromStr for Cuboid {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut it = s.splitn(3, ',');
        let xr = it
            .next()
            .ok_or(InputError::ParseGeneral)
            .and_then(MyInclusiveRange::from_str)?;
        let yr = it
            .next()
            .ok_or(InputError::ParseGeneral)
            .and_then(MyInclusiveRange::from_str)?;
        let zr = it
            .next()
            .ok_or(InputError::ParseGeneral)
            .and_then(MyInclusiveRange::from_str)?;

        Ok(Cuboid { xr, yr, zr })
    }
}

impl Cuboid {
    pub fn new(xs: i32, xe: i32, ys: i32, ye: i32, zs: i32, ze: i32) -> Self {
        Self {
            xr: MyInclusiveRange { start: xs, end: xe },
            yr: MyInclusiveRange { start: ys, end: ye },
            zr: MyInclusiveRange { start: zs, end: ze },
        }
    }
    pub fn overlap(&self, other: &Cuboid) -> Option<Self> {
        if let Some(xr) = self.xr.overlap(&other.xr) {
            if let Some(yr) = self.yr.overlap(&other.yr) {
                if let Some(zr) = self.zr.overlap(&other.zr) {
                    return Some(Self { xr, yr, zr });
                }
            }
        }
        None
    }
}

#[derive(Debug)]
pub struct RebootStep {
    state: State,
    cuboid: Cuboid,
}

impl FromStr for RebootStep {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((s, c)) = s.split_once(' ') {
            Ok(RebootStep {
                state: s.parse()?,
                cuboid: c.parse()?,
            })
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}

type Grid = HashMap<(i32, i32, i32), State>;
impl RebootStep {
    pub fn apply(&self, grid: &mut Grid, area: &Cuboid) {
        if let Some(overlap) = self.cuboid.overlap(area) {
            for x in overlap.xr.start..=overlap.xr.end {
                for y in overlap.yr.start..=overlap.yr.end {
                    for z in overlap.zr.start..=overlap.zr.end {
                        grid.entry((x, y, z))
                            .and_modify(|s| *s = self.state)
                            .or_insert(self.state);
                    }
                }
            }
        }
    }
}
