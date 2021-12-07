use parsing::InputError;
use std::ops::{Add, Sub};
use std::str::FromStr;

#[derive(Debug, Clone, Copy, PartialEq, Hash, Eq)]
pub struct Point {
    pub x: i32,
    pub y: i32,
}

#[derive(Debug)]
pub struct Line {
    start: Point,
    end: Point,
    slope: Point,
}

impl Add for Point {
    type Output = Point;

    fn add(self, rhs: Self) -> Self::Output {
        Point {
            x: self.x + rhs.x,
            y: self.y + rhs.y,
        }
    }
}

impl Sub for Point {
    type Output = Point;

    fn sub(self, rhs: Self) -> Self::Output {
        Point {
            x: self.x - rhs.x,
            y: self.y - rhs.y,
        }
    }
}

impl Point {
    pub fn slope(from: Point, to: Point) -> Point {
        let d = to - from;
        let div = num_integer::gcd(d.x, d.y);
        Point {
            x: d.x / div,
            y: d.y / div,
        }
    }
}

impl Line {
    pub fn is_vertical(&self) -> bool {
        self.start.x == self.end.x
    }

    pub fn is_horizontal(&self) -> bool {
        self.start.y == self.end.y
    }

    pub fn is_diagonal(&self) -> bool {
        !self.is_vertical() && !self.is_horizontal()
    }

    pub fn points(&self) -> LineIter {
        LineIter {
            line: self,
            cur: Some(self.start),
        }
    }
}

pub struct LineIter<'a> {
    line: &'a Line,
    cur: Option<Point>,
}

impl<'a> Iterator for LineIter<'a> {
    type Item = Point;

    fn next(&mut self) -> Option<Self::Item> {
        let ret = self.cur;
        if let Some(p) = self.cur {
            if p == self.line.end {
                self.cur = None;
            } else {
                self.cur = Some(p + self.line.slope);
            }
        }
        ret
    }
}

impl FromStr for Point {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((l, r)) = s.trim().split_once(',') {
            let x = l.parse::<i32>()?;
            let y = r.parse::<i32>()?;
            Ok(Point { x, y })
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}

impl FromStr for Line {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((l, r)) = s.split_once(" -> ") {
            let start = l.parse::<Point>()?;
            let end = r.parse::<Point>()?;
            let slope = Point::slope(start, end);
            Ok(Line { start, end, slope })
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}
