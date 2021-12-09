use parsing::InputError;
use std::fmt::Display;

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
            if row > 0 {
                let top = self.data[row - 1][col];
                is_low_point = is_low_point && top > val;
            }
            if col > 0 {
                let right = self.data[row][col - 1];
                is_low_point = is_low_point && right > val;
            }
            if row < self.height - 1 {
                let bottom = self.data[row + 1][col];
                is_low_point = is_low_point && bottom > val;
            }
            if col < self.width - 1 {
                let left = self.data[row][col + 1];
                is_low_point = is_low_point && left > val;
            }
            is_low_point
        } else {
            false
        }
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
