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

pub trait CardGame {
    fn play(&mut self) -> usize {
        while !self.is_over() {
            self.round();
        }
        self.winner()
    }

    fn is_over(&self) -> bool {
        self.decks()[0].cards.is_empty() || self.decks()[1].cards.is_empty()
    }

    fn winner(&self) -> usize {
        if self.decks()[0].cards.is_empty() {
            1
        } else {
            0
        }
    }

    fn calculate_deck_score(deck: &Deck) -> Score {
        let mut score = 0;
        for (i, card) in deck.cards.iter().rev().enumerate() {
            score += (i + 1) as Score * *card as Score;
        }
        score
    }

    fn score(&self) -> Score {
        if self.is_over() {
            Self::calculate_deck_score(&self.decks()[self.winner()])
        } else {
            0
        }
    }

    fn decks(&self) -> &[Deck];
    fn round(&mut self);
}

pub struct CombatGame {
    decks: [Deck; 2],
}

impl CardGame for CombatGame {
    fn round(&mut self) {
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
}

impl CardGame for RecursiveCombat {
    fn decks(&self) -> &[Deck] {
        &self.decks
    }

    fn round(&mut self) {
        if !self.is_over() {
            let scores = (
                Self::calculate_deck_score(&self.decks[0]),
                Self::calculate_deck_score(&self.decks[1]),
            );
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
                    let mut new_game = RecursiveCombat::new(new_deck_a, new_deck_b);
                    winner = new_game.play();
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
        }
    }

    fn is_over(&self) -> bool {
        self.winner.is_some() || self.decks[0].cards.is_empty() || self.decks[1].cards.is_empty()
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
