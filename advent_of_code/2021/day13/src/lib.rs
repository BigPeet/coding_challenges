use parsing::InputError;
use std::collections::HashSet;
use std::fmt::Display;
use std::str::FromStr;

type SparseMatrix = HashSet<Point>;

pub struct TransparentPaper {
    data: SparseMatrix,
    height: usize,
    width: usize,
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
        TransparentPaper::create_from_sparse_matrix(SparseMatrix::from_iter(points.into_iter()))
    }

    fn create_from_sparse_matrix(points: SparseMatrix) -> TransparentPaper {
        let height = points
            .iter()
            .map(|p| p.y)
            .max()
            .and_then(|m| Some(m + 1))
            .unwrap_or(0);
        let width = points
            .iter()
            .map(|p| p.x)
            .max()
            .and_then(|m| Some(m + 1))
            .unwrap_or(0);
        TransparentPaper {
            data: points,
            height,
            width,
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
        TransparentPaper::create_from_sparse_matrix(new_data)
    }

    pub fn visible_dots(&self) -> usize {
        self.data.len()
    }
}

impl Display for TransparentPaper {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let mut characters = vec![vec!['.'; self.width]; self.height];
        for p in self.data.iter() {
            characters[p.y][p.x] = '#';
        }
        for row in characters.iter() {
            for c in row.iter() {
                write!(f, "{}", c)?;
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
