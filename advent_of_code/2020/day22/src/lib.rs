use parsing::InputError;
use std::collections::VecDeque;
use std::str::FromStr;

type Card = u8;
type Score = u64;

#[derive(Debug, Clone)]
pub struct Deck {
    cards: VecDeque<Card>,
}

impl Deck {
    fn new(cards: &[Card]) -> Deck {
        Deck {
            cards: VecDeque::from_iter(cards.iter().copied()),
        }
    }
    pub fn draw(&mut self) -> Option<Card> {
        self.cards.pop_front()
    }

    pub fn add(&mut self, card: Card) {
        self.cards.push_back(card)
    }

    fn score(&self) -> Score {
        let mut score = 0;
        for (i, card) in self.cards.iter().rev().enumerate() {
            score += (i + 1) as Score * *card as Score;
        }
        score
    }
}

impl FromStr for Deck {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut cards = VecDeque::with_capacity(s.lines().count() - 1);
        for line in s.lines().skip(1) {
            cards.push_back(line.trim_end().parse::<Card>()?);
        }
        Ok(Deck { cards })
    }
}

pub struct FinishedGame {
    winner: usize,
    winning_deck: Deck,
}

impl FinishedGame {
    pub fn score(&self) -> Score {
        self.winning_deck.score()
    }
}

pub enum Outcome<T>
where
    T: CardGame + Sized,
{
    Running(T),
    Win(FinishedGame),
}

pub trait CardGame {
    fn play(mut self) -> FinishedGame
    where
        Self: Sized,
    {
        loop {
            match self.round() {
                Outcome::Running(next) => self = next,
                Outcome::Win(w) => return w,
            }
        }
    }

    fn is_over(&self) -> bool {
        self.decks()[0].cards.is_empty() || self.decks()[1].cards.is_empty()
    }

    fn decks(&self) -> &[Deck];
    fn round(self) -> Outcome<Self>
    where
        Self: Sized;
}

pub struct CombatGame {
    decks: [Deck; 2],
}

impl CardGame for CombatGame {
    fn round(mut self) -> Outcome<Self> {
        if !self.is_over() {
            let card_a = self.decks[0].draw().unwrap();
            let card_b = self.decks[1].draw().unwrap();
            if card_a > card_b {
                self.decks[0].add(card_a);
                self.decks[0].add(card_b);
            } else {
                self.decks[1].add(card_b);
                self.decks[1].add(card_a);
            }
            Outcome::Running(self)
        } else {
            let winner = self.winner();
            Outcome::Win(FinishedGame {
                winner,
                winning_deck: self.decks[winner].clone(),
            })
        }
    }

    fn decks(&self) -> &[Deck] {
        &self.decks
    }
}

impl CombatGame {
    pub fn new(a: Deck, b: Deck) -> CombatGame {
        CombatGame { decks: [a, b] }
    }
    fn winner(&self) -> usize {
        if self.decks()[0].cards.is_empty() {
            1
        } else {
            0
        }
    }
}

pub struct RecursiveCombat {
    decks: [Deck; 2],
    // Instead of storing the full decks, store a "hash" value.
    // In this case: the score.
    history: Vec<(Score, Score)>,
    winner: Option<usize>,
}

impl RecursiveCombat {
    pub fn new(a: Deck, b: Deck) -> RecursiveCombat {
        RecursiveCombat {
            decks: [a, b],
            history: vec![],
            winner: None,
        }
    }
    fn winner(&self) -> usize {
        if let Some(win_id) = self.winner {
            win_id
        } else if self.decks[0].cards.is_empty() {
            1
        } else {
            0
        }
    }
}

impl CardGame for RecursiveCombat {
    fn decks(&self) -> &[Deck] {
        &self.decks
    }

    fn round(mut self) -> Outcome<Self> {
        if !self.is_over() {
            let scores = (self.decks[0].score(), self.decks[1].score());
            if self.history.contains(&scores) {
                // Player 1 (:= 0) wins
                self.winner = Some(0);
            } else {
                self.history.push(scores);
                let drawn_cards = [self.decks[0].draw().unwrap(), self.decks[1].draw().unwrap()];
                let winner;
                if drawn_cards[0] as usize <= self.decks[0].cards.len()
                    && drawn_cards[1] as usize <= self.decks[1].cards.len()
                {
                    // Start subgame
                    let new_deck_a = Deck::new(
                        &self.decks[0].cards.make_contiguous()[0..drawn_cards[0] as usize],
                    );
                    let new_deck_b = Deck::new(
                        &self.decks[1].cards.make_contiguous()[0..drawn_cards[1] as usize],
                    );
                    let new_game = RecursiveCombat::new(new_deck_a, new_deck_b);
                    winner = new_game.play().winner;
                } else {
                    // Normal round
                    winner = if drawn_cards[0] > drawn_cards[1] {
                        0
                    } else {
                        1
                    };
                }
                self.decks[winner].add(drawn_cards[winner]);
                self.decks[winner].add(drawn_cards[1 - winner]);
            }
            Outcome::Running(self)
        } else {
            let winner = self.winner();
            Outcome::Win(FinishedGame {
                winner,
                winning_deck: self.decks[winner].clone(),
            })
        }
    }

    fn is_over(&self) -> bool {
        self.winner.is_some() || self.decks[0].cards.is_empty() || self.decks[1].cards.is_empty()
    }
}
