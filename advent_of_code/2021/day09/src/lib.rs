use parsing::InputError;
use std::collections::VecDeque;
use std::fmt::Display;
use std::ops::Add;

pub struct HeightMap {
    data: Vec<Vec<u8>>,
    height: usize,
    width: usize,
}

impl HeightMap {
    pub fn new(lines: Vec<String>) -> Result<HeightMap, InputError> {
        if lines.is_empty() {
            return Err(InputError::ParseGeneral);
        }
        let height = lines.len();
        let width = lines[0].trim_end().len(); // assuming same length for all strings
        let mut data = Vec::with_capacity(height);
        for line in lines {
            let mut row = Vec::with_capacity(width);
            for c in line.trim_end().chars() {
                if let Some(val) = c.to_digit(10) {
                    row.push(val as u8);
                } else {
                    return Err(InputError::ParseGeneral);
                }
            }
            data.push(row);
        }
        Ok(HeightMap {
            data,
            height,
            width,
        })
    }

    fn is_low_point(&self, row: usize, col: usize) -> bool {
        if row < self.height && col < self.width {
            let val = self.data[row][col];
            let mut is_low_point = true;
            for (dr, dc) in self.neighbours(row, col) {
                let neighbour = self.data[dr][dc];
                is_low_point = is_low_point && neighbour > val;
            }
            is_low_point
        } else {
            false
        }
    }

    fn neighbours(&self, row: usize, col: usize) -> NeighbourIter {
        NeighbourIter::new(row, col, self.height, self.width)
    }

    pub fn low_points(&self) -> Vec<u8> {
        let mut points = vec![];
        for (i, row) in self.data.iter().enumerate() {
            for (j, val) in row.iter().enumerate() {
                if self.is_low_point(i, j) {
                    points.push(*val);
                }
            }
        }
        points
    }

    fn basin_size(&self, row: usize, col: usize) -> u32 {
        let mut q = VecDeque::new();
        let mut seen = vec![(row, col)];
        let mut bsize = 0;
        q.push_back((row, col));
        while let Some((r, c)) = q.pop_front() {
            bsize += 1;
            for (nr, nc) in self.neighbours(r, c) {
                if !seen.contains(&(nr, nc)) {
                    seen.push((nr, nc));
                    let nval = self.data[nr][nc];
                    if nval < 9 {
                        q.push_back((nr, nc));
                    }
                }
            }
        }
        bsize
    }

    pub fn basins(&self) -> Vec<u32> {
        let mut basin_sizes = vec![];
        for i in 0..self.height {
            for j in 0..self.width {
                if self.is_low_point(i, j) {
                    basin_sizes.push(self.basin_size(i, j));
                }
            }
        }
        basin_sizes
    }
}

pub fn risk_levels(mut low_points: Vec<u8>) -> Vec<u8> {
    low_points.iter_mut().for_each(|lp| *lp += 1);
    low_points
}

impl Display for HeightMap {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for row in self.data.iter() {
            for val in row.iter() {
                write!(f, "{}", val)?;
            }
            writeln!(f)?;
        }
        write!(f, "")
    }
}

#[derive(Clone, Copy)]
struct Point {
    x: i32,
    y: i32,
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

struct NeighbourIter {
    center: Point,
    index: usize,
    height: i32,
    width: i32,
}

impl NeighbourIter {
    const OFFSETS: [Point; 4] = [
        Point { x: 0, y: -1 },
        Point { x: -1, y: 0 },
        Point { x: 1, y: 0 },
        Point { x: 0, y: 1 },
    ];

    fn new(row: usize, col: usize, height: usize, width: usize) -> NeighbourIter {
        NeighbourIter {
            center: Point {
                x: col as i32,
                y: row as i32,
            },
            index: 0,
            height: height as i32,
            width: width as i32,
        }
    }
}

impl Iterator for NeighbourIter {
    type Item = (usize, usize);

    fn next(&mut self) -> Option<Self::Item> {
        while self.index < Self::OFFSETS.len() {
            let offset = Self::OFFSETS[self.index];
            self.index += 1;
            let newp = self.center + offset;

            if newp.x >= 0 && newp.x < self.width && newp.y >= 0 && newp.y < self.height {
                return Some((newp.y as usize, newp.x as usize));
            }
        }
        None
    }
}
