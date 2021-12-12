use parsing::InputError;
use std::collections::VecDeque;
use std::str::FromStr;

type Card = u8;
type Score = u64;

#[derive(Debug)]
pub struct Deck {
    cards: VecDeque<Card>,
}

impl Deck {
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

pub struct CombatGame {
    decks: [Deck; 2],
}

impl CombatGame {
    pub fn new(a: Deck, b: Deck) -> CombatGame {
        CombatGame { decks: [a, b] }
    }

    pub fn is_over(&self) -> bool {
        self.decks[0].cards.is_empty() || self.decks[1].cards.is_empty()
    }

    pub fn round(&mut self) {
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

    fn calculate_deck_score(deck: &Deck) -> Score {
        let mut score = 0;
        for (i, card) in deck.cards.iter().rev().enumerate() {
            score += (i + 1) as Score * *card as Score;
        }
        score
    }

    pub fn score(&self) -> Score {
        if self.is_over() {
            let winner;
            if !self.decks[0].cards.is_empty() {
                winner = &self.decks[0];
            } else {
                winner = &self.decks[1];
            }
            Self::calculate_deck_score(winner)
        } else {
            0
        }
    }
}
