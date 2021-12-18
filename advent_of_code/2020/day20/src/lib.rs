use parsing::InputError;
use std::collections::{HashMap, VecDeque};
use std::fmt::Display;
use std::mem;
use std::str::FromStr;

pub trait Transform {
    fn apply(&mut self, transform: Transformation) {
        match transform {
            Transformation::Rotation => self.rotate(),
            Transformation::HorizontalFlip => self.flip_horizontically(),
            Transformation::VerticalFlip => self.flip_vertically(),
        }
    }

    fn reverse(&mut self, transform: Transformation) {
        match transform {
            Transformation::Rotation => self.rotate_left(),
            Transformation::HorizontalFlip => self.flip_horizontically(),
            Transformation::VerticalFlip => self.flip_vertically(),
        }
    }

    // 90 degree rotation to the right
    // assuming NxN images (square)
    fn rotate(&mut self);

    fn rotate_left(&mut self);

    // top -> bottom and vice versa
    fn flip_horizontically(&mut self);

    // left -> right and vice versa
    fn flip_vertically(&mut self);
}

pub trait TransformData {
    type Value;
    fn data_mut(&mut self) -> &mut Vec<Vec<Self::Value>>;
    fn transpose(&mut self) {
        let v = self.data_mut();
        let n = v.len();
        for row in 0..n {
            for col in row + 1..n {
                // swap entry [row][col] with [col][row]
                let (upper, lower) = v.split_at_mut(col);
                mem::swap(&mut upper[row][col], &mut lower[0][row]);
            }
        }
    }
}

impl<T: TransformData> Transform for T {
    // 90 degree rotation to the right
    // assuming NxN images (square)
    fn rotate(&mut self) {
        self.transpose();
        self.flip_vertically();
    }

    fn rotate_left(&mut self) {
        self.flip_vertically();
        self.transpose();
    }

    // top -> bottom and vice versa
    fn flip_horizontically(&mut self) {
        let v = self.data_mut();
        let rows = v.len();
        for r in 0..rows / 2 {
            v.swap(r, rows - 1 - r);
        }
    }

    // left -> right and vice versa
    fn flip_vertically(&mut self) {
        let v = self.data_mut();
        for row in v.iter_mut() {
            let cols = row.len();
            for c in 0..cols / 2 {
                row.swap(c, cols - 1 - c);
            }
        }
    }
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub enum Direction {
    Top,
    Right,
    Bottom,
    Left,
}

impl Direction {
    pub fn new(val: usize) -> Direction {
        let val = val.rem_euclid(4);
        match val {
            0 => Direction::Top,
            1 => Direction::Right,
            2 => Direction::Bottom,
            3 => Direction::Left,
            _ => Direction::Top, // FIXME: infeasible
        }
    }
    fn value(&self) -> i32 {
        match self {
            Direction::Top => 0,
            Direction::Right => 1,
            Direction::Bottom => 2,
            Direction::Left => 3,
        }
    }

    pub fn diff(&self, other: Direction) -> usize {
        (other.value() - self.value()).rem_euclid(4) as usize
    }

    pub fn add(&self, other: Direction) -> usize {
        (other.value() + self.value()).rem_euclid(4) as usize
    }

    pub fn invert(&self) -> Direction {
        match self {
            Direction::Top => Direction::Bottom,
            Direction::Right => Direction::Left,
            Direction::Bottom => Direction::Top,
            Direction::Left => Direction::Right,
        }
    }
}

impl Transform for Direction {
    fn rotate(&mut self) {
        match self {
            Direction::Top => *self = Direction::Right,
            Direction::Right => *self = Direction::Bottom,
            Direction::Bottom => *self = Direction::Left,
            Direction::Left => *self = Direction::Top,
        }
    }

    fn rotate_left(&mut self) {
        match self {
            Direction::Top => *self = Direction::Left,
            Direction::Right => *self = Direction::Top,
            Direction::Bottom => *self = Direction::Right,
            Direction::Left => *self = Direction::Bottom,
        }
    }

    fn flip_horizontically(&mut self) {
        match self {
            Direction::Top => *self = Direction::Bottom,
            Direction::Bottom => *self = Direction::Top,
            _ => (),
        }
    }

    fn flip_vertically(&mut self) {
        match self {
            Direction::Right => *self = Direction::Left,
            Direction::Left => *self = Direction::Right,
            _ => (),
        }
    }
}

#[derive(Debug, Clone, Copy)]
pub enum Transformation {
    Rotation,
    HorizontalFlip,
    VerticalFlip,
}

impl Transformation {
    pub fn needed_rotations(from: Direction, to: Direction) -> Vec<Transformation> {
        let mut rots = vec![];
        for _ in 0..from.diff(to) {
            rots.push(Transformation::Rotation);
        }
        rots
    }
}

#[derive(Debug, Clone, Copy)]
pub struct Match(Direction, bool, usize);

impl Transform for Match {
    fn rotate(&mut self) {
        self.0.rotate();
        // Check if rotation flipped border order
        match self.0 {
            Direction::Top | Direction::Bottom => self.1 = !self.1,
            _ => (),
        }
    }

    fn rotate_left(&mut self) {
        self.0.rotate_left();
        // Check if rotation flipped border order
        match self.0 {
            Direction::Left | Direction::Right => self.1 = !self.1,
            _ => (),
        }
    }

    fn flip_horizontically(&mut self) {
        self.0.flip_horizontically();
        match self.0 {
            Direction::Left | Direction::Right => self.1 = !self.1,
            _ => (),
        }
    }

    fn flip_vertically(&mut self) {
        self.0.flip_vertically();
        match self.0 {
            Direction::Top | Direction::Bottom => self.1 = !self.1,
            _ => (),
        }
    }
}

#[derive(Debug, Default, Clone)]
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
    fn get_borders(&self) -> [String; 4] {
        let mut borders: [String; 4] = Default::default();
        // top (left to right)
        borders[0] = self.data.first().unwrap().iter().collect();
        // right (top to bottom)
        borders[1] = self.data.iter().map(|row| row.last().unwrap()).collect();
        // bottom (left to right)
        borders[2] = self.data.last().unwrap().iter().collect();
        // left (top to bottom)
        borders[3] = self.data.iter().map(|row| row.first().unwrap()).collect();

        borders
    }

    pub fn matches(&self, other: &Tile) -> Option<Match> {
        if self == other {
            return None;
        }
        let self_borders = self.get_borders();
        let other_borders = other.get_borders();
        for (i, border) in self_borders.iter().enumerate() {
            if other_borders.contains(border) {
                return Some(Match(Direction::new(i), false, other.id));
            } else {
                let flipped: String = border.chars().rev().collect();
                if other_borders.contains(&flipped) {
                    return Some(Match(Direction::new(i), true, other.id));
                }
            }
        }
        None
    }

    pub fn id(&self) -> usize {
        self.id
    }
}

impl TransformData for Tile {
    type Value = char;
    fn data_mut(&mut self) -> &mut Vec<Vec<char>> {
        &mut self.data
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

#[derive(Debug)]
pub struct TileImage {
    size: usize,
    tile_size: usize,
    data: Vec<Vec<Tile>>,
}

impl TileImage {
    pub fn assemble(
        mut tile_map: HashMap<usize, Tile>,
        mut match_map: HashMap<usize, Vec<Match>>,
    ) -> Result<TileImage, InputError> {
        let mut q = VecDeque::new();
        let mut visited = vec![];
        let size = (tile_map.values().len() as f32).sqrt() as usize;
        let tile_size;

        // FIXME: clear up the "unwraps" => better error handling

        // Initialize queue with one of the corners as fix point.
        // To make this "deterministic", alway choose the corner with lowest ID.
        if let Some((id, matches)) = match_map
            .iter()
            .filter(|(_, m)| m.len() == 2)
            .min_by_key(|(id, _)| **id)
        {
            // Check which corner
            let match_sides: Vec<Direction> = matches.iter().map(|m| m.0).collect();
            let mut coord: (usize, usize) = (0, 0); // top left
            if match_sides.contains(&Direction::Left) {
                // right corner piece
                coord.1 = size - 1;
            }
            if match_sides.contains(&Direction::Top) {
                // bottom corner piece
                coord.0 = size - 1;
            }
            tile_size = tile_map.get(id).unwrap().data.len();
            q.push_back((*id, coord, Vec::<Transformation>::new()));
            visited.push(*id);
        } else {
            return Err(InputError::ParseGeneral);
        }

        let mut data = vec![vec![Tile::default(); size]; size];

        while !q.is_empty() {
            let (id, coord, transformations) = q.pop_front().unwrap();

            // Get tile and apply transformations.
            let mut tile = tile_map.remove(&id).unwrap();
            transformations
                .iter()
                .for_each(|transform| tile.apply(*transform));

            // Store tile at the coord in the TileImage data.
            data[coord.0][coord.1] = tile;

            // Get matches and "transform" them as well.
            let mut matches = match_map.remove(&id).unwrap();
            for m in matches.iter_mut() {
                transformations
                    .iter()
                    .for_each(|transform| m.apply(*transform));
            }

            // Get matched tiles (that haven't been visited), calculate required transformations and coord.
            for m in matches.iter() {
                if visited.contains(&m.2) {
                    // already in queue or finished
                    continue;
                }
                let mut new_coord = coord;
                match m.0 {
                    Direction::Top => new_coord.0 -= 1,
                    Direction::Bottom => new_coord.0 += 1,
                    Direction::Right => new_coord.1 += 1,
                    Direction::Left => new_coord.1 -= 1,
                }
                let other_id = m.2;
                let other_matches = match_map.get(&other_id).unwrap();
                if let Some(other_match) = other_matches.iter().find(|m| m.2 == id) {
                    let goal = m.0.invert();
                    let m_dir = other_match.0;
                    let mut match_transformations = Transformation::needed_rotations(m_dir, goal);
                    let mut transformed_match = *other_match;
                    match_transformations
                        .iter()
                        .for_each(|transform| transformed_match.apply(*transform));

                    // Before the transformations, the "flip required" must have been equal in both
                    // matches.
                    // Determine if a flip is necessary.
                    //
                    // Was a flip necessary in the original matching? => initially_necessary (bool)
                    // Which tile was flipped through its transformations? None, one or both?
                    // If initially no flip was necessary, but only one got flipped => flip.
                    // If initially a flip was necessary and both/none were flipped => flip.
                    let initially_necessary = other_match.1;
                    let only_one_flipped = m.1 ^ transformed_match.1;
                    if initially_necessary ^ only_one_flipped {
                        match transformed_match.0 {
                            Direction::Left | Direction::Right => {
                                match_transformations.push(Transformation::HorizontalFlip)
                            }
                            Direction::Top | Direction::Bottom => {
                                match_transformations.push(Transformation::VerticalFlip)
                            }
                        }
                    }

                    // add to queue
                    q.push_back((other_id, new_coord, match_transformations));
                    visited.push(other_id);
                }
            }
        }
        Ok(TileImage {
            size,
            tile_size,
            data,
        })
    }
}

impl TransformData for TileImage {
    type Value = Tile;
    fn data_mut(&mut self) -> &mut Vec<Vec<Self::Value>> {
        &mut self.data
    }
}

impl Display for TileImage {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for row in self.data.iter() {
            for t in row.iter() {
                write!(f, " {} ", t.id())?;
            }
            writeln!(f)?;
        }
        write!(f, "")
    }
}

pub struct Image {
    size: usize,
    data: Vec<Vec<char>>,
}

impl Image {
    const MONSTER_DELTAS: [(i32, i32); 15] = [
        (0, 0),
        (1, 0),
        (1, 1),
        (1, -1),
        (1, -6),
        (1, -7),
        (1, -12),
        (1, -13),
        (1, -18),
        (2, -2),
        (2, -5),
        (2, -8),
        (2, -11),
        (2, -14),
        (2, -17),
    ];
    // Takes coordinates for the monster's "head" (see below)
    //
    //                   # <--- "head"
    // #    ##    ##    ###
    //  #  #  #  #  #  #
    //
    // From the head's view there need to be
    // - 2 rows below the head
    // - 1 column to its right
    // - 18 columns to its left
    // - total width: 20 columns,
    // - total height: 3 rows
    fn is_monster(&self, row: usize, col: usize) -> bool {
        for delta in Self::MONSTER_DELTAS {
            let pos = (row as i32 + delta.0, col as i32 + delta.1);
            let pos = (pos.0 as usize, pos.1 as usize);
            if self.data[pos.0][pos.1] != '#' {
                return false;
            }
        }
        true
    }

    fn search_monsters(&mut self) -> usize {
        let mut count = 0;
        // possible head positions
        for row in 0..self.size - 2 {
            for col in 18..self.size - 1 {
                if self.is_monster(row, col) {
                    self.mark_monster((row, col));
                    count += 1;
                }
            }
        }
        count
    }

    fn mark_monster(&mut self, position: (usize, usize)) {
        for delta in Self::MONSTER_DELTAS {
            let pos = (position.0 as i32 + delta.0, position.1 as i32 + delta.1);
            let pos = (pos.0 as usize, pos.1 as usize);
            self.data[pos.0][pos.1] = 'O';
        }
    }

    // Returns a scanned image, which highlights the monsters with Os and contains the total count
    // of monsters found.
    pub fn scan_for_monsters(mut self) -> ScannedImage {
        let mut count = 0;
        // look at all four rotations
        for _ in 0..4 {
            count += self.search_monsters();

            // flip horizontically (and back)
            self.flip_horizontically();
            count += self.search_monsters();
            self.flip_horizontically();

            // flip vertically (and back)
            self.flip_vertically();
            count += self.search_monsters();
            self.flip_vertically();

            // next rotation
            self.rotate();
        }
        ScannedImage(self, count)
    }
}

impl From<TileImage> for Image {
    fn from(tile_image: TileImage) -> Self {
        let tile_size = tile_image.tile_size - 2;
        let image_len = tile_size * tile_image.size;
        let mut data = vec![vec!['.'; image_len]; image_len];
        for (i, row) in tile_image.data.into_iter().enumerate() {
            for (j, tile) in row.into_iter().enumerate() {
                for (r, tile_row) in tile.data.into_iter().skip(1).take(tile_size).enumerate() {
                    for (c, val) in tile_row.into_iter().skip(1).take(tile_size).enumerate() {
                        data[i * tile_size + r][j * tile_size + c] = val;
                    }
                }
            }
        }
        Image {
            size: image_len,
            data,
        }
    }
}

impl Display for Image {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for row in self.data.iter() {
            for c in row.iter() {
                write!(f, "{}", c)?;
            }
            writeln!(f)?;
        }
        write!(f, "")
    }
}

impl TransformData for Image {
    type Value = char;

    fn data_mut(&mut self) -> &mut Vec<Vec<Self::Value>> {
        &mut self.data
    }
}

pub struct ScannedImage(Image, usize);

impl ScannedImage {
    pub fn water_roughness(&self) -> usize {
        let mut count = 0;
        for row in self.0.data.iter() {
            count += row.iter().filter(|c| **c == '#').count();
        }
        count
    }

    pub fn count(&self) -> usize {
        self.1
    }
}

impl Display for ScannedImage {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.0)
    }
}
