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

pub struct Probe {
    pos: Position,
    velocity: Velocity,
}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Velocity(Vec2);
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub struct Position(Vec2);

impl Default for Position {
    fn default() -> Self {
        Position(Vec2 { x: 0, y: 0 })
    }
}

impl Position {
    // Does not handle cases in which the area is intersecting the x or y axis
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

    pub fn is_going_upward(&self) -> bool {
        self.0.y > 0
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
            pos: Position(Vec2 { x: 0, y: 0 }),
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
}

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

    fn horizontal_value_range(&self) -> (i32, i32) {
        let min_x = self.find_min_x();
        let max_x = self.bottom_far.x;
        if min_x < max_x {
            (min_x, max_x)
        } else {
            (max_x, min_x)
        }
    }

    // This assumes the area is in the bottom quadrant.
    fn vertical_value_range(&self) -> (i32, i32) {
        // For any y-value > 0, the probe will come back to the x-axis (y=0) with exactly -y velocity.
        // If y is greater than the lower bound, then it will miss.
        (self.lower_bound(), -self.lower_bound())
    }

    fn lower_bound(&self) -> i32 {
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

pub fn viable_velocities(target: &Area) -> Vec<Velocity> {
    let mut viable_velocities = vec![];
    let (x_from, x_to) = target.horizontal_value_range();
    let (y_from, y_to) = target.vertical_value_range();
    for x in x_from..=x_to {
        for y in y_from..=y_to {
            let vel = Velocity::new(x, y);
            let mut probe = Probe::new(vel);
            while !probe.pos().is_in_area(&target) && !probe.pos().has_missed(&target) {
                probe.step();
            }
            if probe.pos().is_in_area(&target) {
                viable_velocities.push(vel);
            }
        }
    }
    viable_velocities
}

pub fn find_highest_position(velocities: &[Velocity]) -> Option<i32> {
    let mut max_v = *velocities.iter().max_by_key(|v| v.0.y)?;
    let mut height = 0;
    while max_v.is_going_upward() {
        height += max_v.0.y;
        max_v.update();
    }
    Some(height)
}
