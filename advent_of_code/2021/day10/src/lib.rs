use parsing::InputError;
use std::fmt::Display;
use std::str::FromStr;

#[derive(Clone, Copy)]
enum Delimiter {
    Paren,
    Bracket,
    Brace,
    Angled,
}

#[derive(Clone, Copy)]
pub struct Symbol {
    delim: Delimiter,
    is_left: bool,
}

impl Symbol {
    const NUM_OF_DELIMITERS: usize = 4;
    const LEFTS: [char; Self::NUM_OF_DELIMITERS] = ['(', '[', '{', '<'];
    const RIGHTS: [char; Self::NUM_OF_DELIMITERS] = [')', ']', '}', '>'];

    fn value(&self) -> usize {
        self.delim as usize
    }

    fn is_open(&self) -> bool {
        self.is_left
    }

    fn get_closing(&self) -> Symbol {
        Symbol {
            delim: self.delim,
            is_left: false,
        }
    }

    pub fn error_score(&self) -> u64 {
        if !self.is_left {
            match self.delim {
                Delimiter::Paren => 3,
                Delimiter::Bracket => 57,
                Delimiter::Brace => 1197,
                Delimiter::Angled => 25137,
            }
        } else {
            0
        }
    }

    pub fn completion_score(&self) -> u64 {
        if !self.is_left {
            match self.delim {
                Delimiter::Paren => 1,
                Delimiter::Bracket => 2,
                Delimiter::Brace => 3,
                Delimiter::Angled => 4,
            }
        } else {
            0
        }
    }
}

impl Display for Symbol {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let c = if self.is_left {
            Self::LEFTS[self.delim as usize]
        } else {
            Self::RIGHTS[self.delim as usize]
        };
        write!(f, "{}", c)
    }
}

impl TryFrom<char> for Symbol {
    type Error = InputError;

    fn try_from(value: char) -> Result<Self, Self::Error> {
        let symbols;
        let is_left = Self::LEFTS.contains(&value);
        if is_left {
            symbols = Self::LEFTS;
        } else if Self::RIGHTS.contains(&value) {
            symbols = Self::RIGHTS;
        } else {
            return Err(InputError::ParseGeneral);
        }

        let pos = symbols.iter().position(|c| *c == value);
        let delim = match pos {
            Some(0) => Delimiter::Paren,
            Some(1) => Delimiter::Bracket,
            Some(2) => Delimiter::Brace,
            Some(3) => Delimiter::Angled,
            _ => return Err(InputError::ParseGeneral),
        };

        Ok(Symbol { delim, is_left })
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
        let mut stack = vec![];
        for sym in self.0.iter() {
            if sym.is_open() {
                // open a new chunk
                stack.push(*sym);
            } else {
                // close a chunk
                stack.pop();
            }
        }

        let mut score = 0;
        // Reverse the remaining stack.
        // (should only include unclosed open symbols)
        while let Some(popped) = stack.pop() {
            let closing_sym = popped.get_closing();
            self.0.push(closing_sym);
            score *= 5;
            score += closing_sym.completion_score();
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
