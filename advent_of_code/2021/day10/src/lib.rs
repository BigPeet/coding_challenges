use parsing::InputError;
use std::fmt::Display;
use std::str::FromStr;

#[derive(Clone, Copy)]
pub enum Symbol {
    LParen,
    RParen,
    LBracket,
    RBracket,
    LBrace,
    RBrace,
    LAngled,
    RAngled,
}

impl Symbol {
    const LPAREN: char = '(';
    const RPAREN: char = ')';
    const LBRACKET: char = '[';
    const RBRACKET: char = ']';
    const LBRACE: char = '{';
    const RBRACE: char = '}';
    const LANGLED: char = '<';
    const RANGLED: char = '>';
    const NUM_OF_DELIMITERS: usize = 4;

    fn value(&self) -> usize {
        match self {
            Symbol::LParen | Symbol::RParen => 0,
            Symbol::LBracket | Symbol::RBracket => 1,
            Symbol::LBrace | Symbol::RBrace => 2,
            Symbol::LAngled | Symbol::RAngled => 3,
        }
    }

    fn is_open(&self) -> bool {
        match self {
            Symbol::LParen | Symbol::LBracket | Symbol::LBrace | Symbol::LAngled => true,
            Symbol::RParen | Symbol::RBracket | Symbol::RBrace | Symbol::RAngled => false,
        }
    }

    fn get_closing(value: usize) -> Option<Symbol> {
        match value {
            0 => Some(Symbol::RParen),
            1 => Some(Symbol::RBracket),
            2 => Some(Symbol::RBrace),
            3 => Some(Symbol::RAngled),
            _ => None,
        }
    }

    pub fn error_score(&self) -> u64 {
        match self {
            Symbol::RParen => 3,
            Symbol::RBracket => 57,
            Symbol::RBrace => 1197,
            Symbol::RAngled => 25137,
            _ => 0,
        }
    }

    pub fn completion_score(&self) -> u64 {
        match self {
            Symbol::RParen => 1,
            Symbol::RBracket => 2,
            Symbol::RBrace => 3,
            Symbol::RAngled => 4,
            _ => 0,
        }
    }
}

impl Display for Symbol {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let c = match self {
            Symbol::LParen => Symbol::LPAREN,
            Symbol::RParen => Symbol::RPAREN,
            Symbol::LBracket => Symbol::LBRACKET,
            Symbol::RBracket => Symbol::RBRACKET,
            Symbol::LBrace => Symbol::LBRACE,
            Symbol::RBrace => Symbol::RBRACE,
            Symbol::LAngled => Symbol::LANGLED,
            Symbol::RAngled => Symbol::RANGLED,
        };
        write!(f, "{}", c)
    }
}

impl TryFrom<char> for Symbol {
    type Error = InputError;

    fn try_from(value: char) -> Result<Self, Self::Error> {
        match value {
            Symbol::LPAREN => Ok(Symbol::LParen),
            Symbol::RPAREN => Ok(Symbol::RParen),
            Symbol::LBRACKET => Ok(Symbol::LBracket),
            Symbol::RBRACKET => Ok(Symbol::RBracket),
            Symbol::LBRACE => Ok(Symbol::LBrace),
            Symbol::RBRACE => Ok(Symbol::RBrace),
            Symbol::LANGLED => Ok(Symbol::LAngled),
            Symbol::RANGLED => Ok(Symbol::RAngled),
            _ => Err(InputError::ParseGeneral),
        }
    }
}

pub struct SymbolLine(Vec<Symbol>);

impl SymbolLine {
    pub fn find_illegal(&self) -> Option<Symbol> {
        let mut levels = [0i32; Symbol::NUM_OF_DELIMITERS];
        let mut stack = vec![];
        for sym in self.0.iter() {
            let idx = sym.value();
            if sym.is_open() {
                // open a new chunk
                stack.push(levels);
                levels[idx] += 1;
            } else {
                // close a chunk
                levels[idx] -= 1;
                // levels must be equal to levels when this chunk was created
                let popped = stack.pop();
                if popped.is_none() || popped.unwrap() != levels {
                    return Some(*sym);
                }
            }
        }
        None
    }

    fn complete(&mut self) -> u64 {
        let mut levels = [0i32; Symbol::NUM_OF_DELIMITERS];
        let mut stack = vec![];
        for sym in self.0.iter() {
            let idx = sym.value();
            if sym.is_open() {
                // open a new chunk
                levels[idx] += 1;
                stack.push(levels);
            } else {
                // close a chunk
                stack.pop();
                levels[idx] -= 1;
            }
        }

        let mut score = 0;
        // Reverse the remaining stack.
        while let Some(popped) = stack.pop() {
            // which symbol was added
            let prev;
            if let Some(last) = stack.last() {
                prev = last;
            } else {
                prev = &[0i32; Symbol::NUM_OF_DELIMITERS];
            }
            for i in 0..Symbol::NUM_OF_DELIMITERS {
                if prev[i] < popped[i] {
                    // this was opened most recently
                    let closing_sym = Symbol::get_closing(i).unwrap();
                    self.0.push(closing_sym);
                    score *= 5;
                    score += closing_sym.completion_score();
                    break;
                }
            }
        }
        score
    }
}

impl Display for SymbolLine {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for sym in self.0.iter() {
            write!(f, "{}", sym)?;
        }
        write!(f, "")
    }
}

impl FromStr for SymbolLine {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut symbols = Vec::with_capacity(s.len());
        for c in s.chars() {
            let symbol = c.try_into()?;
            symbols.push(symbol);
        }
        Ok(SymbolLine(symbols))
    }
}

pub struct IncompleteLines(Vec<SymbolLine>);
pub struct CompleteLines(Vec<SymbolLine>);

impl From<Vec<SymbolLine>> for IncompleteLines {
    fn from(lines: Vec<SymbolLine>) -> Self {
        let only_incomplete = lines
            .into_iter()
            .filter(|sl| sl.find_illegal().is_none())
            .collect();
        IncompleteLines(only_incomplete)
    }
}

impl IncompleteLines {
    pub fn complete(mut self) -> (CompleteLines, u64) {
        let mut scores = Vec::with_capacity(self.0.len());
        for sl in self.0.iter_mut() {
            scores.push(sl.complete());
        }
        scores.sort_unstable();
        let score = scores[scores.len() / 2]; // guaranteed to be of odd length
        (CompleteLines(self.0), score)
    }
}
