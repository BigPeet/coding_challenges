use parsing::InputError;
use std::fmt::Display;
use std::str::FromStr;

enum Amphoid {
    Amber,
    Bronze,
    Copper,
    Desert,
}

impl TryFrom<u8> for Amphoid {
    type Error = InputError;

    fn try_from(value: u8) -> Result<Self, Self::Error> {
        let c = value as char;
        match c {
            'A' => Ok(Amphoid::Amber),
            'B' => Ok(Amphoid::Bronze),
            'C' => Ok(Amphoid::Copper),
            'D' => Ok(Amphoid::Desert),
            _ => Err(InputError::ParseGeneral),
        }
    }
}

enum Tile {
    Empty,
    Occupied(Amphoid),
}

impl Default for Tile {
    fn default() -> Self {
        Self::Empty
    }
}

struct Room {
    tiles: [Tile; 2],
}

impl Room {
    fn occupied(top: Amphoid, bottom: Amphoid) -> Self {
        Self {
            tiles: [Tile::Occupied(top), Tile::Occupied(bottom)],
        }
    }
}

pub struct Burrow {
    hallway: [Tile; 11],
    rooms: [Room; 4],
}

impl FromStr for Burrow {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let lines = s.lines().collect::<Vec<&str>>();
        if lines.len() == 5 {
            let top_bytes = lines[2].as_bytes();
            let bot_bytes = lines[3].as_bytes();
            if top_bytes.len() == 13 && bot_bytes.len() == 11 {
                let rooms = [
                    Room::occupied(top_bytes[3].try_into()?, bot_bytes[3].try_into()?),
                    Room::occupied(top_bytes[5].try_into()?, bot_bytes[5].try_into()?),
                    Room::occupied(top_bytes[7].try_into()?, bot_bytes[7].try_into()?),
                    Room::occupied(top_bytes[9].try_into()?, bot_bytes[9].try_into()?),
                ];
                return Ok(Self {
                    hallway: Default::default(),
                    rooms,
                });
            }
        }
        Err(InputError::ParseGeneral)
    }
}

impl Display for Amphoid {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Amphoid::Amber => write!(f, "A"),
            Amphoid::Bronze => write!(f, "B"),
            Amphoid::Copper => write!(f, "C"),
            Amphoid::Desert => write!(f, "D"),
        }
    }
}

impl Display for Tile {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Tile::Empty => write!(f, "."),
            Tile::Occupied(a) => write!(f, "{}", a),
        }
    }
}

impl Display for Burrow {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        writeln!(f, "#############")?;
        write!(f, "#")?;
        for t in self.hallway.iter() {
            write!(f, "{}", t)?;
        }
        writeln!(f, "#")?;
        writeln!(
            f,
            "###{}#{}#{}#{}###",
            self.rooms[0].tiles[0],
            self.rooms[1].tiles[0],
            self.rooms[2].tiles[0],
            self.rooms[3].tiles[0]
        )?;
        writeln!(
            f,
            "  #{}#{}#{}#{}#",
            self.rooms[0].tiles[1],
            self.rooms[1].tiles[1],
            self.rooms[2].tiles[1],
            self.rooms[3].tiles[1]
        )?;
        write!(f, "  #########")
    }
}
