pub trait Die {
    fn roll(&mut self) -> u32;
    fn roll_thrice(&mut self) -> u32 {
        self.roll() + self.roll() + self.roll()
    }

    fn total_rolls(&self) -> u32;
}

pub struct DeterministicDie {
    face: u32,
    rolls: u32,
}

impl DeterministicDie {
    const MAX: u32 = 100;

    pub fn new() -> Self {
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
        Self {
            pos: pos - 1,
            score: 0,
        }
    }

    fn advance(&mut self, steps: u32) {
        self.pos = (self.pos + steps).rem_euclid(10);
        self.score += self.pos + 1;
    }
}

pub struct DiracGame<const MAX_SCORE: u32> {
    player_1: Player,
    player_2: Player,
}

impl<const MAX_SCORE: u32> DiracGame<MAX_SCORE> {
    fn new(p1_start: u32, p2_start: u32) -> Self {
        Self {
            player_1: Player::new(p1_start),
            player_2: Player::new(p2_start),
        }
    }

    fn turn(&mut self, player_idx: u32, steps: u32) -> bool {
        let player = if player_idx == 1 {
            &mut self.player_1
        } else {
            &mut self.player_2
        };
        player.advance(steps);
        player.score >= MAX_SCORE
    }

    pub fn play_with_deterministic_die(mut self, die: &mut DeterministicDie) -> (u32, u32) {
        while !self.turn(1, die.roll_thrice()) && !self.turn(2, die.roll_thrice()) {}

        (
            self.player_1.score.max(self.player_2.score),
            self.player_1.score.min(self.player_2.score),
        )
    }
}

impl DiracGame<1000> {
    pub fn part1(p1_start: u32, p2_start: u32) -> Self {
        Self::new(p1_start, p2_start)
    }
}

impl DiracGame<21> {
    pub fn part2(p1_start: u32, p2_start: u32) -> Self {
        Self::new(p1_start, p2_start)
    }
}
