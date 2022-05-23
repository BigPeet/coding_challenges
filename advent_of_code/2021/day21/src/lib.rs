use std::fmt::Debug;

pub trait Die {
    fn roll(&mut self) -> u32;
    fn roll_thrice(&mut self) -> u32 {
        self.roll() + self.roll() + self.roll()
    }

    fn total_rolls(&self) -> u32;
}

#[derive(Debug, Copy, Clone)]
pub struct DeterministicDie {
    face: u32,
    rolls: u32,
}

impl DeterministicDie {
    const MAX: u32 = 100;

    fn new() -> Self {
        Self { face: 0, rolls: 0 }
    }
}

impl Default for DeterministicDie {
    fn default() -> Self {
        Self::new()
    }
}

impl Die for DeterministicDie {
    fn roll(&mut self) -> u32 {
        if self.face == Self::MAX {
            self.face = 0
        }
        self.face += 1;
        self.rolls += 1;
        self.face
    }

    fn total_rolls(&self) -> u32 {
        self.rolls
    }
}

struct Player {
    pos: u32,
    score: u32,
}

impl Player {
    fn new(pos: u32) -> Self {
        Self { pos, score: 0 }
    }
}

pub struct DiracGame<T> {
    player_1: Player,
    player_2: Player,
    die: T,
}

impl<T> DiracGame<T>
where
    T: Die + Default + Debug,
{
    pub fn new(p1_start: u32, p2_start: u32) -> Self {
        Self {
            player_1: Player::new(p1_start),
            player_2: Player::new(p2_start),
            die: T::default(),
        }
    }

    pub fn play(&mut self) {
        while !self.turn(1) && !self.turn(2) {}
    }

    pub fn loser_score(&self) -> u32 {
        self.player_1.score.min(self.player_2.score)
    }

    pub fn total_rolls(&self) -> u32 {
        self.die.total_rolls()
    }

    fn turn(&mut self, player_idx: u32) -> bool {
        let player = if player_idx == 1 {
            &mut self.player_1
        } else {
            &mut self.player_2
        };

        let steps: u32 = self.die.roll_thrice();
        let mut new_pos = player.pos + steps;
        if new_pos > 10 {
            new_pos = new_pos.rem_euclid(10);
            if new_pos == 0 {
                new_pos = 10;
            }
        }

        player.score += new_pos;
        player.pos = new_pos;
        player.score >= 1000
    }
}

impl DiracGame<DeterministicDie> {
    pub fn with_deterministic_die(p1_start: u32, p2_start: u32) -> Self {
        DiracGame::new(p1_start, p2_start)
    }
}
