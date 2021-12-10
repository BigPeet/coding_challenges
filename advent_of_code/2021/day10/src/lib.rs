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

    pub fn score(&self) -> u32 {
        match self {
            Symbol::RParen => 3,
            Symbol::RBracket => 57,
            Symbol::RBrace => 1197,
            Symbol::RAngled => 25137,
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
