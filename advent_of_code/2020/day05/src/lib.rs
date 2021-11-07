use parsing::InputError;
use std::str::FromStr;

pub struct Seat {
    row: u8,
    col: u8,
}

impl Seat {
    pub fn id(&self) -> usize {
        self.row as usize * 8 + self.col as usize
    }
}

impl FromStr for Seat {
    type Err = InputError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if s.len() == 10 {
            let (row, col) = s.split_at(7);
            let row: String = row
                .chars()
                .map(|c| if c == 'F' { '0' } else { '1' })
                .collect();
            let col: String = col
                .chars()
                .map(|c| if c == 'L' { '0' } else { '1' })
                .collect();
            let row = u8::from_str_radix(row.as_str(), 2)?;
            let col = u8::from_str_radix(col.as_str(), 2)?;
            return Ok(Seat { row, col });
        }
        Err(Self::Err::ParseGeneral)
    }
}
