use parsing::InputError;
use std::collections::HashSet;
use std::fmt::Display;
use std::str::FromStr;

type SparseMatrix = HashSet<Point>;

pub struct TransparentPaper {
    data: SparseMatrix,
}

#[derive(PartialEq, Eq, Hash)]
pub struct Point {
    x: usize,
    y: usize,
}

pub enum FoldInstruction {
    Horizontal(usize),
    Vertical(usize),
}

impl TransparentPaper {
    pub fn new(points: Vec<Point>) -> TransparentPaper {
        TransparentPaper {
            data: SparseMatrix::from_iter(points.into_iter()),
        }
    }

    pub fn apply(self, instruction: &FoldInstruction) -> TransparentPaper {
        // FIXME: a little wasteful because we create a new hashset every time.
        let mut new_data = SparseMatrix::new();
        for mut point in self.data.into_iter() {
            match *instruction {
                FoldInstruction::Horizontal(row) => {
                    if point.y > row {
                        point.y = row - (point.y - row);
                    }
                }
                FoldInstruction::Vertical(col) => {
                    if point.x > col {
                        point.x = col - (point.x - col);
                    }
                }
            }
            new_data.insert(point);
        }
        TransparentPaper { data: new_data }
    }

    pub fn visible_dots(&self) -> usize {
        self.data.len()
    }
}

impl Display for TransparentPaper {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let height = self
            .data
            .iter()
            .map(|p| p.y)
            .max()
            .map(|m| m + 1)
            .unwrap_or(0);
        let width = self
            .data
            .iter()
            .map(|p| p.x)
            .max()
            .map(|m| m + 1)
            .unwrap_or(0);
        for y in 0..height {
            for x in 0..width {
                let p = Point { x, y };
                if self.data.contains(&p) {
                    write!(f, "#")?;
                } else {
                    write!(f, ".")?;
                }
            }
            writeln!(f)?;
        }
        write!(f, "")
    }
}

impl FromStr for Point {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((l, r)) = s.trim_end().split_once(',') {
            Ok(Point {
                x: l.parse::<usize>()?,
                y: r.parse::<usize>()?,
            })
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}

impl FromStr for FoldInstruction {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let assign = s.trim_start_matches("fold along ");
        if let Some((l, r)) = assign.trim().split_once('=') {
            let value = r.parse::<usize>()?;
            if l == "y" {
                Ok(FoldInstruction::Horizontal(value))
            } else if l == "x" {
                Ok(FoldInstruction::Vertical(value))
            } else {
                Err(InputError::ParseGeneral)
            }
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}
