use parsing::InputError;
use std::str::FromStr;

const BOARD_SIZE: usize = 5;

#[derive(Debug)]
pub struct BingoBoard {
    numbers: [[(u32, bool); BOARD_SIZE]; BOARD_SIZE],
}

pub struct IterBoard<'a> {
    board: &'a BingoBoard,
    row: usize,
    col: usize,
}

pub struct IterMutBoard<'a> {
    board: &'a mut BingoBoard,
    row: usize,
    col: usize,
}

impl BingoBoard {
    pub fn iter(&self) -> IterBoard {
        IterBoard {
            board: self,
            row: 0,
            col: 0,
        }
    }

    pub fn iter_mut(&mut self) -> IterMutBoard {
        IterMutBoard {
            board: self,
            row: 0,
            col: 0,
        }
    }

    pub fn mark(&mut self, value: u32) {
        for (val, selected) in self.iter_mut() {
            if *val == value {
                *selected = true;
            }
        }
    }

    pub fn finished(&self) -> bool {
        // rows
        for row in self.numbers.iter() {
            if row.iter().all(|(_, sel)| *sel) {
                return true;
            }
        }

        // cols
        for col in 0..BOARD_SIZE {
            if self.numbers.iter().map(|row| row[col].1).all(|sel| sel) {
                return true;
            }
        }
        false
    }

    pub fn unmarked(&self) -> u32 {
        self.iter()
            .filter_map(|(val, sel)| if !sel { Some(val) } else { None })
            .sum()
    }
}

impl FromStr for BingoBoard {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let lines: Vec<&str> = s.split('\n').collect();
        if lines.len() != BOARD_SIZE {
            return Err(InputError::ParseGeneral);
        }

        let mut data = [[(0, false); BOARD_SIZE]; BOARD_SIZE];
        for (i, line) in lines.into_iter().enumerate() {
            let line: Vec<&str> = line.split_whitespace().collect();
            if line.len() != BOARD_SIZE {
                return Err(InputError::ParseGeneral);
            }
            for (j, c) in line.into_iter().enumerate() {
                data[i][j] = (c.parse::<u32>()?, false);
            }
        }

        Ok(BingoBoard { numbers: data })
    }
}

impl<'a> Iterator for IterBoard<'a> {
    type Item = &'a (u32, bool);

    fn next(&mut self) -> Option<Self::Item> {
        if self.col >= BOARD_SIZE {
            self.col = 0;
            self.row += 1;
        }
        if self.row >= BOARD_SIZE {
            None
        } else {
            let col = self.col;
            self.col += 1;
            Some(&self.board.numbers[self.row][col])
        }
    }
}

impl<'a> Iterator for IterMutBoard<'a> {
    type Item = &'a mut (u32, bool);

    fn next(&mut self) -> Option<Self::Item> {
        if self.col >= BOARD_SIZE {
            self.col = 0;
            self.row += 1;
        }
        if self.row >= BOARD_SIZE {
            None
        } else {
            let col = self.col;
            self.col += 1;
            let ptr = self.board.numbers[self.row].as_mut_ptr();
            unsafe { Some(&mut *ptr.add(col)) }
        }
    }
}
