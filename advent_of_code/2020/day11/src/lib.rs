use std::fmt::Display;

#[derive(Clone)]
enum Tile {
    Floor,
    FreeSeat,
    OccupiedSeat,
}

impl Tile {
    pub fn occupied(&self) -> bool {
        matches!(self, Tile::OccupiedSeat)
    }
}

pub struct SeatingPlan {
    width: usize,
    height: usize,
    layout: Vec<Vec<Tile>>,
    pub occupied_limit: usize,
    pub use_visibility: bool,
}

impl Display for SeatingPlan {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let mut text = String::new();
        for row in self.layout.iter() {
            for entry in row.iter() {
                let c = match entry {
                    Tile::Floor => '.',
                    Tile::FreeSeat => 'L',
                    Tile::OccupiedSeat => '#',
                };
                text.push(c);
            }
            text.push('\n');
        }
        write!(f, "{}", text)
    }
}

impl SeatingPlan {
    pub fn new(lines: &[String]) -> Self {
        if !lines.is_empty() {
            let height = lines.len();
            let width = lines[0].len(); // assuming all lines have the same length
            let mut layout = Vec::with_capacity(height);
            for line in lines.iter() {
                let mut row = Vec::with_capacity(width);
                for c in line.chars() {
                    match c {
                        '.' => row.push(Tile::Floor),
                        'L' => row.push(Tile::FreeSeat),
                        '#' => row.push(Tile::OccupiedSeat),
                        _ => continue,
                    }
                }
                layout.push(row);
            }

            Self {
                width,
                height,
                layout,
                occupied_limit: 4,
                use_visibility: false,
            }
        } else {
            Self {
                width: 0,
                height: 0,
                layout: Vec::new(),
                occupied_limit: 4,
                use_visibility: false,
            }
        }
    }

    fn check_neighbour(&self, row: usize, col: usize, delta: (i32, i32)) -> usize {
        let mut neighbour_row = row as i32 + delta.0;
        let mut neighbour_col = col as i32 + delta.1;

        while neighbour_row >= 0
            && (neighbour_row as usize) < self.height
            && neighbour_col >= 0
            && (neighbour_col as usize) < self.width
        {
            match self.layout[neighbour_row as usize][neighbour_col as usize] {
                Tile::OccupiedSeat => return 1,
                Tile::FreeSeat => return 0,
                Tile::Floor => {
                    if self.use_visibility {
                        neighbour_row += delta.0;
                        neighbour_col += delta.1;
                    } else {
                        return 0;
                    }
                }
            }
        }
        0
    }

    fn check_neighbours(&self, row: usize, col: usize) -> usize {
        let mut occupied = 0;
        occupied += self.check_neighbour(row, col, (-1, -1));
        occupied += self.check_neighbour(row, col, (-1, 0));
        occupied += self.check_neighbour(row, col, (-1, 1));

        occupied += self.check_neighbour(row, col, (0, -1));
        occupied += self.check_neighbour(row, col, (0, 1));

        occupied += self.check_neighbour(row, col, (1, -1));
        occupied += self.check_neighbour(row, col, (1, 0));
        occupied += self.check_neighbour(row, col, (1, 1));
        occupied
    }

    pub fn step(&mut self) -> bool {
        let mut updated = false;
        let mut new_layout = self.layout.clone();
        for (row_index, row) in new_layout.iter_mut().enumerate() {
            for (col_index, tile) in row.iter_mut().enumerate() {
                match self.layout[row_index][col_index] {
                    Tile::FreeSeat if self.check_neighbours(row_index, col_index) == 0 => {
                        *tile = Tile::OccupiedSeat;
                        updated = true
                    }
                    Tile::OccupiedSeat
                        if self.check_neighbours(row_index, col_index) >= self.occupied_limit =>
                    {
                        *tile = Tile::FreeSeat;
                        updated = true
                    }
                    _ => continue,
                }
            }
        }
        self.layout = new_layout;
        updated
    }

    pub fn count_occupied(&self) -> usize {
        self.layout.iter().fold(0, |acc, row| {
            acc + row.iter().filter(|e| e.occupied()).count()
        })
    }
}
