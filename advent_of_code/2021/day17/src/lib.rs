use parsing::InputError;
use std::str::FromStr;

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Vec2 {
    pub x: i32,
    pub y: i32,
}

#[derive(Debug, Clone, Copy)]
pub struct Area {
    top_near: Vec2,
    bottom_far: Vec2,
}

#[derive(Debug, Clone, Copy)]
pub struct Probe {
    pos: Position,
    velocity: Velocity,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Velocity(pub Vec2);
#[derive(Debug, Clone, Copy)]
pub struct Position(pub Vec2);

impl Default for Position {
    fn default() -> Self {
        Position::new(0, 0)
    }
}

impl Position {
    pub fn new(x: i32, y: i32) -> Position {
        Position(Vec2 { x, y })
    }
    pub fn landing(mut v: &mut Velocity, steps: usize) -> Position {
        let mut pos = Position::default();
        (0..steps).for_each(|_| pos.step(&mut v));
        pos
    }
    pub fn step(&mut self, v: &mut Velocity) {
        // Update position
        self.0.x += v.0.x;
        self.0.y += v.0.y;

        // Update velocity
        v.update();
    }

    pub fn is_in_area(&self, target: &Area) -> bool {
        let horizontal;
        let vertical;
        // is the area to the right or left?
        if target.top_near.x > 0 {
            horizontal = self.0.x >= target.top_near.x && self.0.x <= target.bottom_far.x;
        } else {
            horizontal = self.0.x <= target.top_near.x && self.0.x >= target.bottom_far.x;
        }
        // is the area above or below
        if target.top_near.y > 0 {
            vertical = self.0.y >= target.top_near.y && self.0.y <= target.bottom_far.y;
        } else {
            vertical = self.0.y <= target.top_near.y && self.0.y >= target.bottom_far.y;
        }

        horizontal && vertical
    }

    pub fn has_missed(&self, target: &Area) -> bool {
        let horizontal;
        // is the area to the right or left?
        if target.top_near.x > 0 {
            horizontal = self.0.x > target.bottom_far.x;
        } else {
            horizontal = self.0.x < target.bottom_far.x;
        }
        let vertical = self.0.y < target.bottom_far.y;
        horizontal || vertical
    }
}

impl Velocity {
    pub fn new(x: i32, y: i32) -> Velocity {
        Velocity(Vec2 { x, y })
    }

    pub fn only_x(x: i32) -> Velocity {
        Velocity(Vec2 { x, y: 0 })
    }

    pub fn update(&mut self) {
        if self.0.x > 0 {
            self.0.x -= 1;
        } else if self.0.x < 0 {
            self.0.x += 1;
        }
        self.0.y -= 1;
    }
}

impl Probe {
    pub fn new(velocity: Velocity) -> Probe {
        Probe {
            pos: Position (Vec2{ x: 0, y: 0 }),
            velocity,
        }
    }

    pub fn step(&mut self) {
        // Update position
        self.pos.0.x += self.velocity.0.x;
        self.pos.0.y += self.velocity.0.y;

        // Update velocity
        self.velocity.update();
    }

    pub fn steps(&mut self, times: usize) {
        (0..times).for_each(|_| self.step())
    }

    pub fn pos(&self) -> &Position {
        &self.pos
    }

    pub fn vel(&self) -> &Velocity {
        &self.velocity
    }

    // Does not handle cases in which the area is intersecting the x or y axis
    //pub fn is_in_area(&self, target: &Area) -> bool {
        //let horizontal;
        //let vertical;
        //// is the area to the right or left?
        //if target.top_near.x > 0 {
            //horizontal = self.pos.x >= target.top_near.x && self.pos.x <= target.bottom_far.x;
        //} else {
            //horizontal = self.pos.x <= target.top_near.x && self.pos.x >= target.bottom_far.x;
        //}
        //// is the area above or below
        //if target.top_near.y > 0 {
            //vertical = self.pos.y >= target.top_near.y && self.pos.y <= target.bottom_far.y;
        //} else {
            //vertical = self.pos.y <= target.top_near.y && self.pos.y >= target.bottom_far.y;
        //}

        //horizontal && vertical
    //}

}

//pub fn hits_target(velocity: Vec2, target: &Area) -> (Vec2, bool) {
    //let mut probe = Probe::new(Velocity::new(velocity.x, velocity.y));
    //while !probe.is_in_area(target) && !probe.has_missed(target) {
        //probe.step();
    //}
    //(probe.pos, probe.is_in_area(target))
//}

impl Area {
    // This assumes the area is not intersecting with the x or y axis.
    pub fn new(x1: i32, x2: i32, y1: i32, y2: i32) -> Area {
        // x1 -> x2 is left to right
        // y1 -> y2 is bottom to top
        let near_x;
        let far_x;
        if x2 > 0 {
            near_x = x1;
            far_x = x2;
        } else {
            near_x = x2;
            far_x = x1;
        }

        Area {
            top_near: Vec2 { x: near_x, y: y2 },
            bottom_far: Vec2 { x: far_x, y: y1 },
        }
    }

    fn find_min_x(&self) -> i32 {
        let bound = self.top_near.x.abs();
        for x in 0..bound {
            let mut dx = x;
            let mut sum = 0;
            while dx != 0 {
                sum += dx;
                dx -= 1;
            }
            if sum >= bound {
                return self.top_near.x.signum() * x;
            }
        }
        0
    }

    fn horizontal_match(&self, x: i32) -> bool {
        let x_abs = x.abs();
        x_abs >= self.top_near.x.abs() && x_abs <= self.bottom_far.x.abs()
    }

    pub fn hit_candidates(&self, x: i32) -> Vec<usize> {
        let mut steps = vec![];
        if x.signum() == self.top_near.x.signum() {
            let mut v = Velocity::only_x(x);
            let mut lands = v.0.x;
            let mut step = 1;
            while v.0.x != 0 {
                v.update();
                if self.horizontal_match(lands) {
                    steps.push(step);
                }
                lands += v.0.x;
                step += 1;
            }
        }
        steps
    }

    pub fn velocity_x_values(&self) -> (i32, i32) {
        let min_x = self.find_min_x();
        let max_x = self.bottom_far.x;
        if min_x < max_x {
            (min_x, max_x)
        } else {
            (max_x, min_x)
        }
    }

    pub fn lower_bound(&self) -> i32 {
        self.bottom_far.y
    }
}

impl FromStr for Area {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let coords = s.trim_start_matches("target area: ").trim_end();
        if let Some((l, r)) = coords.split_once(", ") {
            // x
            if let Some((xl, xr)) = l.split_once("..") {
                let xl = &xl[2..]; // skip x=
                let x1 = xl.parse::<i32>()?;
                let x2 = xr.parse::<i32>()?;
                // y
                if let Some((yl, yr)) = r.split_once("..") {
                    let yl = &yl[2..]; // skip y=
                    let y1 = yl.parse::<i32>()?;
                    let y2 = yr.parse::<i32>()?;

                    return Ok(Area::new(x1, x2, y1, y2));
                }
            }
        }

        Err(InputError::ParseGeneral)
    }
}
