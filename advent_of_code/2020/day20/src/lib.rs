use parsing::InputError;
use std::fmt::Display;
use std::str::FromStr;

#[derive(Debug, Clone)]
pub struct Tile {
    id: usize,
    data: Vec<Vec<char>>,
}

impl PartialEq for Tile {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

impl Tile {
    // 90 degree rotation to the right
    // assuming NxN images (square)
    pub fn rotate(&mut self) {
        let old = self.data.clone();
        let length = self.data.len();
        for (r, row) in old.iter().enumerate() {
            for (c, val) in row.iter().enumerate() {
                self.data[c][length - r - 1] = *val;
            }
        }
    }

    // top -> bottom and vice versa
    pub fn flip_horizontically(&mut self) {
        let old = self.data.clone();
        let length = self.data.len();
        for (r, row) in old.iter().enumerate() {
            for (c, val) in row.iter().enumerate() {
                self.data[length - r - 1][c] = *val;
            }
        }
    }

    // left -> right and vice versa
    pub fn flip_vertically(&mut self) {
        let old = self.data.clone();
        let length = self.data.len();
        for (r, row) in old.iter().enumerate() {
            for (c, val) in row.iter().enumerate() {
                self.data[r][length - 1 - c] = *val;
            }
        }
    }

    fn get_borders(&self) -> [String; 4] {
        let mut borders: [String; 4] = Default::default();
        // top
        borders[0] = self.data.first().unwrap().iter().collect();
        // right
        borders[1] = self.data.iter().map(|row| row.last().unwrap()).collect();
        // bottom
        borders[2] = self.data.last().unwrap().iter().collect();
        // left
        borders[3] = self.data.iter().map(|row| row.first().unwrap()).collect();

        borders
    }

    // TODO: replace bool with some Match-Struct/Option, describing required transformation, etc.
    pub fn matches(&self, other: &Tile) -> bool {
        let self_borders = self.get_borders();
        let other_borders = other.get_borders();
        for border in self_borders.iter() {
            if other_borders.contains(border) {
                return true;
            } else {
                let flipped: String = border.chars().rev().collect();
                if other_borders.contains(&flipped) {
                    return true;
                }
            }
        }
        false
    }

    pub fn id(&self) -> usize {
        self.id
    }
}

impl Display for Tile {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        writeln!(f, "Tile {}:", self.id)?;
        for row in self.data.iter() {
            for c in row.iter() {
                write!(f, "{}", c)?;
            }
            writeln!(f)?;
        }
        write!(f, "")
    }
}

impl FromStr for Tile {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((header, content)) = s.split_once('\n') {
            // ID "Tile XYZ:"
            let id = header.split_once(' ').ok_or(InputError::ParseGeneral)?.1;
            let id = id[..id.len() - 1].parse::<usize>()?;
            let mut data = vec![];
            for l in content.lines() {
                let mut row = vec![];
                for c in l.trim().chars() {
                    row.push(c);
                }
                data.push(row);
            }

            Ok(Tile { id, data })
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}
