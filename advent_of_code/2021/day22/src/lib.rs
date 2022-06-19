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

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
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
            Ok(MyInclusiveRange::new(s, e))
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}

impl MyInclusiveRange {
    fn new(start: i32, end: i32) -> Self {
        assert!(start <= end);
        Self { start, end }
    }

    fn overlap(&self, other: &Self) -> Option<Self> {
        if self.end < other.start || self.start > other.end {
            // There is no overlap.
            None
        } else {
            let start = self.start.max(other.start);
            let end = self.end.min(other.end);
            Some(MyInclusiveRange::new(start, end))
        }
    }

    fn contains(&self, other: &MyInclusiveRange) -> bool {
        self.start <= other.start && self.end >= other.end
    }

    fn split_ranges(&self, other: &MyInclusiveRange) -> Vec<MyInclusiveRange> {
        if other.contains(self) {
            return vec![*self];
        } else if self.contains(other) {
            let mut ranges = vec![];
            if self.start < other.start {
                ranges.push(MyInclusiveRange::new(self.start, other.start - 1));
            }
            ranges.push(*other);
            if self.end > other.end {
                ranges.push(MyInclusiveRange::new(other.end + 1, self.end));
            }
            ranges
        } else if self.start < other.start {
            return vec![
                MyInclusiveRange::new(self.start, other.start - 1),
                MyInclusiveRange::new(other.start, self.end),
            ];
        } else {
            return vec![
                MyInclusiveRange::new(self.start, other.end),
                MyInclusiveRange::new(other.end + 1, self.end),
            ];
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
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
    fn combine(&self, other: &Cuboid) -> Option<Cuboid> {
        if self == other {
            return None;
        }
        if self.xr.start == (other.xr.end + 1) && self.yr == other.yr && self.zr == other.zr {
            // left
            return Some(Cuboid {
                xr: MyInclusiveRange::new(other.xr.start, self.xr.end),
                yr: self.yr,
                zr: self.zr,
            });
        } else if self.xr.end == (other.xr.start + 1) && self.yr == other.yr && self.zr == other.zr
        {
            // right
            return Some(Cuboid {
                xr: MyInclusiveRange::new(self.xr.start, other.xr.end),
                yr: self.yr,
                zr: self.zr,
            });
        } else if self.yr.start == (other.yr.end + 1) && self.xr == other.xr && self.zr == other.zr
        {
            // "below"
            return Some(Cuboid {
                xr: self.xr,
                yr: MyInclusiveRange::new(other.yr.start, self.yr.end),
                zr: self.zr,
            });
        } else if self.yr.end == (other.yr.start + 1) && self.xr == other.xr && self.zr == other.zr
        {
            // "above"
            return Some(Cuboid {
                xr: self.xr,
                yr: MyInclusiveRange::new(self.yr.start, other.yr.end),
                zr: self.zr,
            });
        } else if self.zr.start == (other.zr.end + 1) && self.xr == other.xr && self.yr == other.yr
        {
            // "before"
            return Some(Cuboid {
                xr: self.xr,
                yr: self.yr,
                zr: MyInclusiveRange::new(other.zr.start, self.zr.end),
            });
        } else if self.zr.end == (other.zr.start + 1) && self.xr == other.xr && self.yr == other.yr
        {
            // "after"
            return Some(Cuboid {
                xr: self.xr,
                yr: self.yr,
                zr: MyInclusiveRange::new(self.zr.start, other.zr.end),
            });
        }

        None
    }
    fn compress(mut cuboids: Vec<Cuboid>) -> Vec<Cuboid> {
        if cuboids.len() < 2 {
            return cuboids;
        }
        // Ugly :-/
        let mut new_found = Some((0, 0));
        while new_found.is_some() {
            new_found = None;
            let mut remaining = vec![];
            for i in 0..cuboids.len() {
                let c1 = &cuboids[i];
                for (j, c2) in cuboids.iter().enumerate() {
                    if i != j {
                        if let Some(c12) = c1.combine(c2) {
                            remaining.push(c12);
                            new_found = Some((i, j));
                            break;
                        }
                    }
                }
                if new_found.is_some() {
                    break;
                }
            }
            if let Some((i, j)) = new_found {
                remaining.extend(cuboids.into_iter().enumerate().filter_map(|(idx, c)| {
                    if idx != i && idx != j {
                        Some(c)
                    } else {
                        None
                    }
                }));
                cuboids = remaining;
            }
        }
        cuboids
    }

    fn contains(&self, other: &Cuboid) -> bool {
        self.xr.contains(&other.xr) && self.yr.contains(&other.yr) && self.zr.contains(&other.zr)
    }

    fn merge(self, other: Cuboid) -> Vec<Cuboid> {
        // Case 1: One cube completely contains the other.
        if self.contains(&other) {
            return vec![];
        } else if other.contains(&self) {
            return vec![other];
        }

        // Case 2: There is no overlap.
        if !self.overlaps(&other) {
            return vec![other];
        }

        // Case 3: The cuboids do overlap, but not completely.
        let rem = other.subtract(&self);
        Cuboid::compress(rem)
    }

    pub fn new(xs: i32, xe: i32, ys: i32, ye: i32, zs: i32, ze: i32) -> Self {
        Self {
            xr: MyInclusiveRange::new(xs, xe),
            yr: MyInclusiveRange::new(ys, ye),
            zr: MyInclusiveRange::new(zs, ze),
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

    pub fn overlaps(&self, other: &Cuboid) -> bool {
        self.overlap(other).is_some()
    }

    pub fn split(self, other: &Cuboid) -> Vec<Cuboid> {
        // Case 1: Other cube completely contains self..
        if other.contains(&self) {
            return vec![];
        }

        // Case 2: There is no overlap.
        if !self.overlaps(other) {
            return vec![self];
        }

        // Case 3: The cuboids do overlap.
        let keep = self.subtract(other);
        Cuboid::compress(keep)
    }

    // Removes the overlapping part and creates new cuboids from the remainder.
    fn subtract(&self, other: &Cuboid) -> Vec<Cuboid> {
        let x_ranges = self.xr.split_ranges(&other.xr);
        let y_ranges = self.yr.split_ranges(&other.yr);
        let z_ranges = self.zr.split_ranges(&other.zr);

        let mut result = Vec::with_capacity(x_ranges.len() * y_ranges.len() * z_ranges.len() - 1);

        for &xr in x_ranges.iter() {
            for &yr in y_ranges.iter() {
                for &zr in z_ranges.iter() {
                    let c = Cuboid { xr, yr, zr };
                    if !other.contains(&c) {
                        result.push(c);
                    }
                }
            }
        }
        result
    }

    pub fn volume(&self) -> u64 {
        // stupid off-by-one here (and elsewhere) cost me a long debugging session :P
        let x = (self.xr.end.abs_diff(self.xr.start) + 1) as u64;
        let y = (self.yr.end.abs_diff(self.yr.start) + 1) as u64;
        let z = (self.zr.end.abs_diff(self.zr.start) + 1) as u64;
        x * y * z
    }
}

#[derive(Default)]
pub struct MegaCuboid(Vec<Cuboid>);

impl MegaCuboid {
    pub fn volume(&self) -> u64 {
        self.0.iter().map(|c| c.volume()).sum()
    }

    pub fn len(&self) -> usize {
        self.0.len()
    }

    pub fn is_empty(&self) -> bool {
        self.0.is_empty()
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
    pub fn apply_init(&self, grid: &mut Grid, area: &Cuboid) {
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

    pub fn apply_reboot(&self, cuboids: MegaCuboid) -> MegaCuboid {
        match self.state {
            State::On => self.add(cuboids),
            State::Off => self.remove(cuboids),
        }
    }

    fn add(&self, cuboids: MegaCuboid) -> MegaCuboid {
        if cuboids.0.is_empty() {
            return MegaCuboid(vec![self.cuboid]);
        }
        let mut result = vec![];
        let mut to_add = vec![self.cuboid];
        for c in cuboids.0 {
            if self.cuboid.contains(&c) {
                continue;
            } else {
                result.push(c);
            }
            let mut reduced_add = vec![];
            for candidate in to_add {
                let rem = c.merge(candidate);
                reduced_add.extend(rem);
            }
            to_add = reduced_add;
        }
        result.extend(to_add);
        MegaCuboid(result)
    }

    fn remove(&self, cuboids: MegaCuboid) -> MegaCuboid {
        let mut result = MegaCuboid(vec![]);
        for c in cuboids.0 {
            let keep = c.split(&self.cuboid);
            result.0.extend(keep);
        }
        result
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn substract_z_axis_test() {
        let c1 = Cuboid::new(0, 3, 0, 3, 0, 3);
        let c2 = Cuboid::new(1, 2, 1, 2, -3, 3);
        let rem = c1.subtract(&c2);
        assert_eq!(rem.len(), 8);
    }

    #[test]
    fn substract_y_axis_test() {
        let c1 = Cuboid::new(0, 3, 0, 3, 0, 3);
        let c2 = Cuboid::new(1, 2, -3, 3, 1, 2);
        let rem = c1.subtract(&c2);
        assert_eq!(rem.len(), 8);
    }
    #[test]
    fn substract_x_axis_test() {
        let c1 = Cuboid::new(0, 3, 0, 3, 0, 3);
        let c2 = Cuboid::new(-3, 3, 1, 2, 1, 2);
        let rem = c1.subtract(&c2);
        assert_eq!(rem.len(), 8);
    }
    #[test]
    fn substract_contained_test() {
        let c1 = Cuboid::new(0, 3, 0, 3, 0, 3);
        let c2 = Cuboid::new(1, 2, 1, 2, 1, 2);
        let rem = c1.subtract(&c2);
        assert_eq!(rem.len(), 26);
    }
    #[test]
    fn substract_test_1() {
        let c1 = Cuboid::new(0, 3, 0, 3, 0, 3);
        let c2 = Cuboid::new(2, 3, 0, 2, 0, 3);
        let rem = c1.subtract(&c2);
        assert_eq!(rem.len(), 3);
    }
    #[test]
    fn substract_test_2() {
        let c1 = Cuboid::new(0, 3, 0, 3, 0, 3);
        let c2 = Cuboid::new(2, 3, 0, 2, 0, 2);
        let rem = c1.subtract(&c2);
        assert_eq!(rem.len(), 7);
    }

    #[test]
    fn rs_add_test() {
        let c1 = Cuboid::new(-44, 5, -27, 21, -14, 35);
        let c2 = Cuboid::new(-5, 47, -31, 22, -19, 33);
        dbg!(c2);
        let overlap = c1.overlap(&c2).unwrap();
        let rs = RebootStep {
            state: State::On,
            cuboid: c1,
        };
        dbg!(&rs);
        let cuboids = MegaCuboid(vec![c2]);
        let added = rs.apply_reboot(cuboids);
        for c in added.0.iter() {
            dbg!(c, c.volume());
        }
        assert_eq!(c1.volume() + c2.volume() - overlap.volume(), added.volume());
    }

    #[test]
    fn rs_remove_same_test() {
        let c = Cuboid::new(-44, 5, -27, 21, -14, 35);
        let rs = RebootStep {
            state: State::Off,
            cuboid: c,
        };
        let cuboids = MegaCuboid(vec![c]);
        let removed = rs.apply_reboot(cuboids);
        assert!(removed.0.is_empty());
        assert_eq!(0, removed.volume());
    }

    #[test]
    fn rs_remove_within_test() {
        let c1 = Cuboid::new(1, 49, 1, 49, 1, 49);
        let c2 = Cuboid::new(0, 50, 0, 50, 0, 50);
        let overlap = c2.overlap(&c1).unwrap();
        let rs = RebootStep {
            state: State::Off,
            cuboid: c1,
        };
        let cuboids = MegaCuboid(vec![c2]);
        let removed = rs.apply_reboot(cuboids);
        assert!(!removed.0.is_empty());
        assert_eq!(15002, removed.volume());
        assert_eq!(c2.volume() - overlap.volume(), removed.volume());
    }

    #[test]
    fn test_non_overlapping_remove_test() {
        let init = vec![
            RebootStep::from_str("on x=-5..47,y=-31..22,z=-19..33").unwrap(),
            RebootStep::from_str("on x=-44..5,y=-27..21,z=-14..35").unwrap(),
            RebootStep::from_str("on x=-49..-1,y=-11..42,z=-10..38").unwrap(),
            RebootStep::from_str("on x=-20..34,y=-40..6,z=-44..1").unwrap(),
        ];
        let rs = RebootStep::from_str("off x=26..39,y=40..50,z=-2..11").unwrap();
        let mut cuboids = MegaCuboid(vec![]);
        for rs in init {
            cuboids = rs.apply_reboot(cuboids);
        }
        assert!(cuboids.0.iter().all(|c| !c.overlaps(&rs.cuboid)));

        let before = cuboids.volume();
        cuboids = rs.apply_reboot(cuboids);
        let after = cuboids.volume();

        assert_eq!(before, after);
    }

    #[test]
    fn test_overlapping_remove_test() {
        let init = vec![
            RebootStep::from_str("on x=-5..47,y=-31..22,z=-19..33").unwrap(),
            RebootStep::from_str("on x=-44..5,y=-27..21,z=-14..35").unwrap(),
            RebootStep::from_str("on x=-49..-1,y=-11..42,z=-10..38").unwrap(),
            RebootStep::from_str("on x=-20..34,y=-40..6,z=-44..1").unwrap(),
            RebootStep::from_str("on x=-41..5,y=-41..6,z=-36..8").unwrap(),
        ];
        let rs = RebootStep::from_str("off x=-43..-33,y=-45..-28,z=7..25").unwrap();
        let mut cuboids = MegaCuboid(vec![]);
        for rs in init {
            cuboids = rs.apply_reboot(cuboids);
        }

        assert!(cuboids.0.iter().any(|c| c.overlaps(&rs.cuboid)));

        let mut expected_diff = 0;
        for c in cuboids.0.iter() {
            if let Some(ov) = c.overlap(&rs.cuboid) {
                expected_diff += ov.volume();
            }
        }

        let before = cuboids.volume();
        cuboids = rs.apply_reboot(cuboids);
        let after = cuboids.volume();
        assert!(after < before);
        assert_eq!(after + expected_diff, before);
    }

    #[test]
    fn rs_volume_test() {
        let init = vec![
            RebootStep::from_str("on x=-5..47,y=-31..22,z=-19..33").unwrap(),
            RebootStep::from_str("on x=-44..5,y=-27..21,z=-14..35").unwrap(),
            RebootStep::from_str("on x=-49..-1,y=-11..42,z=-10..38").unwrap(),
            RebootStep::from_str("on x=-20..34,y=-40..6,z=-44..1").unwrap(),
            RebootStep::from_str("off x=26..39,y=40..50,z=-2..11").unwrap(),
            RebootStep::from_str("on x=-41..5,y=-41..6,z=-36..8").unwrap(),
            RebootStep::from_str("off x=-43..-33,y=-45..-28,z=7..25").unwrap(),
            RebootStep::from_str("on x=-33..15,y=-32..19,z=-34..11").unwrap(),
            RebootStep::from_str("off x=35..47,y=-46..-34,z=-11..5").unwrap(),
            RebootStep::from_str("on x=-14..36,y=-6..44,z=-16..29").unwrap(),
        ];

        let mut cuboids = MegaCuboid(vec![]);
        for rs in init {
            let mut expected_diff = match rs.state {
                State::On => -(rs.cuboid.volume() as i64),
                State::Off => 0,
            };
            dbg!(&rs, expected_diff);
            for c in cuboids.0.iter() {
                if let Some(ov) = c.overlap(&rs.cuboid) {
                    expected_diff += ov.volume() as i64;
                    dbg!(&ov, ov.volume(), expected_diff);
                }
            }
            let before = cuboids.volume() as i64;
            cuboids = rs.apply_reboot(cuboids);
            let after = cuboids.volume() as i64;
            dbg!(before, after, expected_diff);
            assert_eq!(after + expected_diff, before);
        }
    }
    #[test]
    fn part_02_test() {
        let steps = vec![
            RebootStep::from_str("on x=-5..47,y=-31..22,z=-19..33").unwrap(),
            RebootStep::from_str("on x=-44..5,y=-27..21,z=-14..35").unwrap(),
            RebootStep::from_str("on x=-49..-1,y=-11..42,z=-10..38").unwrap(),
            RebootStep::from_str("on x=-20..34,y=-40..6,z=-44..1").unwrap(),
            RebootStep::from_str("off x=26..39,y=40..50,z=-2..11").unwrap(),
            RebootStep::from_str("on x=-41..5,y=-41..6,z=-36..8").unwrap(),
            RebootStep::from_str("off x=-43..-33,y=-45..-28,z=7..25").unwrap(),
            RebootStep::from_str("on x=-33..15,y=-32..19,z=-34..11").unwrap(),
            RebootStep::from_str("off x=35..47,y=-46..-34,z=-11..5").unwrap(),
            RebootStep::from_str("on x=-14..36,y=-6..44,z=-16..29").unwrap(),
            RebootStep::from_str("on x=-57795..-6158,y=29564..72030,z=20435..90618").unwrap(),
            RebootStep::from_str("on x=36731..105352,y=-21140..28532,z=16094..90401").unwrap(),
            RebootStep::from_str("on x=30999..107136,y=-53464..15513,z=8553..71215").unwrap(),
            RebootStep::from_str("on x=13528..83982,y=-99403..-27377,z=-24141..23996").unwrap(),
            RebootStep::from_str("on x=-72682..-12347,y=18159..111354,z=7391..80950").unwrap(),
            RebootStep::from_str("on x=-1060..80757,y=-65301..-20884,z=-103788..-16709").unwrap(),
            RebootStep::from_str("on x=-83015..-9461,y=-72160..-8347,z=-81239..-26856").unwrap(),
            RebootStep::from_str("on x=-52752..22273,y=-49450..9096,z=54442..119054").unwrap(),
            RebootStep::from_str("on x=-29982..40483,y=-108474..-28371,z=-24328..38471").unwrap(),
            RebootStep::from_str("on x=-4958..62750,y=40422..118853,z=-7672..65583").unwrap(),
            RebootStep::from_str("on x=55694..108686,y=-43367..46958,z=-26781..48729").unwrap(),
            RebootStep::from_str("on x=-98497..-18186,y=-63569..3412,z=1232..88485").unwrap(),
            RebootStep::from_str("on x=-726..56291,y=-62629..13224,z=18033..85226").unwrap(),
            RebootStep::from_str("on x=-110886..-34664,y=-81338..-8658,z=8914..63723").unwrap(),
            RebootStep::from_str("on x=-55829..24974,y=-16897..54165,z=-121762..-28058").unwrap(),
            RebootStep::from_str("on x=-65152..-11147,y=22489..91432,z=-58782..1780").unwrap(),
            RebootStep::from_str("on x=-120100..-32970,y=-46592..27473,z=-11695..61039").unwrap(),
            RebootStep::from_str("on x=-18631..37533,y=-124565..-50804,z=-35667..28308").unwrap(),
            RebootStep::from_str("on x=-57817..18248,y=49321..117703,z=5745..55881").unwrap(),
            RebootStep::from_str("on x=14781..98692,y=-1341..70827,z=15753..70151").unwrap(),
            RebootStep::from_str("on x=-34419..55919,y=-19626..40991,z=39015..114138").unwrap(),
            RebootStep::from_str("on x=-60785..11593,y=-56135..2999,z=-95368..-26915").unwrap(),
            RebootStep::from_str("on x=-32178..58085,y=17647..101866,z=-91405..-8878").unwrap(),
            RebootStep::from_str("on x=-53655..12091,y=50097..105568,z=-75335..-4862").unwrap(),
            RebootStep::from_str("on x=-111166..-40997,y=-71714..2688,z=5609..50954").unwrap(),
            RebootStep::from_str("on x=-16602..70118,y=-98693..-44401,z=5197..76897").unwrap(),
            RebootStep::from_str("on x=16383..101554,y=4615..83635,z=-44907..18747").unwrap(),
            RebootStep::from_str("off x=-95822..-15171,y=-19987..48940,z=10804..104439").unwrap(),
            RebootStep::from_str("on x=-89813..-14614,y=16069..88491,z=-3297..45228").unwrap(),
            RebootStep::from_str("on x=41075..99376,y=-20427..49978,z=-52012..13762").unwrap(),
            RebootStep::from_str("on x=-21330..50085,y=-17944..62733,z=-112280..-30197").unwrap(),
            RebootStep::from_str("on x=-16478..35915,y=36008..118594,z=-7885..47086").unwrap(),
            RebootStep::from_str("off x=-98156..-27851,y=-49952..43171,z=-99005..-8456").unwrap(),
            RebootStep::from_str("off x=2032..69770,y=-71013..4824,z=7471..94418").unwrap(),
            RebootStep::from_str("on x=43670..120875,y=-42068..12382,z=-24787..38892").unwrap(),
            RebootStep::from_str("off x=37514..111226,y=-45862..25743,z=-16714..54663").unwrap(),
            RebootStep::from_str("off x=25699..97951,y=-30668..59918,z=-15349..69697").unwrap(),
            RebootStep::from_str("off x=-44271..17935,y=-9516..60759,z=49131..112598").unwrap(),
            RebootStep::from_str("on x=-61695..-5813,y=40978..94975,z=8655..80240").unwrap(),
            RebootStep::from_str("off x=-101086..-9439,y=-7088..67543,z=33935..83858").unwrap(),
            RebootStep::from_str("off x=18020..114017,y=-48931..32606,z=21474..89843").unwrap(),
            RebootStep::from_str("off x=-77139..10506,y=-89994..-18797,z=-80..59318").unwrap(),
            RebootStep::from_str("off x=8476..79288,y=-75520..11602,z=-96624..-24783").unwrap(),
            RebootStep::from_str("on x=-47488..-1262,y=24338..100707,z=16292..72967").unwrap(),
            RebootStep::from_str("off x=-84341..13987,y=2429..92914,z=-90671..-1318").unwrap(),
            RebootStep::from_str("off x=-37810..49457,y=-71013..-7894,z=-105357..-13188").unwrap(),
            RebootStep::from_str("off x=-27365..46395,y=31009..98017,z=15428..76570").unwrap(),
            RebootStep::from_str("off x=-70369..-16548,y=22648..78696,z=-1892..86821").unwrap(),
            RebootStep::from_str("on x=-53470..21291,y=-120233..-33476,z=-44150..38147").unwrap(),
            RebootStep::from_str("off x=-93533..-4276,y=-16170..68771,z=-104985..-24507").unwrap(),
        ];
        let mut cuboids = MegaCuboid(vec![]);
        for rs in steps {
            let mut expected_diff = match rs.state {
                State::On => -(rs.cuboid.volume() as i128),
                State::Off => 0,
            };
            dbg!(&rs, expected_diff);
            for c in cuboids.0.iter() {
                if let Some(ov) = c.overlap(&rs.cuboid) {
                    expected_diff += ov.volume() as i128;
                    dbg!(&c, &ov, ov.volume(), expected_diff);
                }
            }
            let before = cuboids.volume() as i128;
            cuboids = rs.apply_reboot(cuboids);
            let after = cuboids.volume() as i128;
            dbg!(before, after, expected_diff);
            assert_eq!(after + expected_diff, before);

            for c1 in cuboids.0.iter() {
                for c2 in cuboids.0.iter() {
                    if c1 != c2 {
                        assert!(!c1.overlaps(c2));
                    }
                }
            }
        }
        let final_v = cuboids.volume();
        assert_eq!(2758514936282235, final_v);
    }
}
