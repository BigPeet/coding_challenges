use parsing::InputError;
use std::fmt::Display;
use std::str::FromStr;

pub enum Expression {
    Value(u64),
    Addition(Box<Expression>, Box<Expression>),
    Multiplication(Box<Expression>, Box<Expression>),
    Parens(Box<Expression>),
}

impl Display for Expression {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Self::Value(val) => write!(f, "{}", val),
            Self::Addition(lhs, rhs) => write!(f, "{} + {}", lhs, rhs),
            Self::Multiplication(lhs, rhs) => write!(f, "{} * {}", lhs, rhs),
            Self::Parens(inner) => write!(f, "({})", inner),
        }
    }
}


impl Expression {
    pub fn evaluate(&self) -> u64 {
        match self {
            Self::Value(val) => *val,
            Self::Addition(lhs, rhs) => lhs.evaluate() + rhs.evaluate(),
            Self::Multiplication(lhs, rhs) => lhs.evaluate() * rhs.evaluate(),
            Self::Parens(inner) => inner.evaluate(),
        }
    }

    fn find_closing_paren(tokens: &[&str], start: usize) -> Option<usize> {
        let mut parens_level = 0;
        for (idx, tok) in tokens[start..].iter().enumerate() {
            match *tok {
                ")" if parens_level == 0 => return Some(idx + start),
                "(" => parens_level += 1,
                ")" => parens_level -= 1,
                _ => (),
            }
        }
        None
    }

    fn parse_parens(tokens: &[&str]) -> Result<Self, InputError> {
        return Ok(Expression::Parens(Box::new(Expression::parse_expression(
            &tokens[1..tokens.len() - 1],
        )?)));
    }

    pub fn parse_expression(tokens: &[&str]) -> Result<Self, InputError> {
        let token_length = tokens.len();
        // Has the expression the form (...) where the outer parens belong together? => parse inner
        if tokens[0] == "("
            && tokens[token_length - 1] == ")"
            && Expression::find_closing_paren(tokens, 1).unwrap_or(0) == token_length - 1
        {
            return Expression::parse_parens(tokens);
        }

        if !tokens.contains(&"+") && !tokens.contains(&"*") {
            // no operation => should only be a value
            // remove any ()
            let filtered: Vec<&&str> = tokens
                .iter()
                .filter(|tok| **tok != "(" && **tok != ")")
                .collect();
            return Ok(Expression::parse_value(&filtered[0])?);
        }

        let mut parens_level = 0;
        let mut idx = token_length;

        for tok in tokens.iter().rev() {
            match *tok {
                ")" => parens_level += 1,
                "(" if parens_level > 0 => parens_level -= 1,
                "(" if parens_level == 0 => return Err(InputError::ParseGeneral),
                "+" if parens_level == 0 && idx > token_length - 1 => {
                    return Err(InputError::ParseGeneral)
                }
                "*" if parens_level == 0 && idx > token_length - 1 => {
                    return Err(InputError::ParseGeneral)
                }
                "+" if parens_level == 0 => {
                    let lhs = Self::parse_expression(&tokens[..idx - 1])?;
                    let rhs = Self::parse_expression(&tokens[idx..])?;
                    return Ok(Self::Addition(Box::new(lhs), Box::new(rhs)));
                }
                "*" if parens_level == 0 => {
                    let lhs = Self::parse_expression(&tokens[..idx - 1])?;
                    let rhs = Self::parse_expression(&tokens[idx..])?;
                    return Ok(Self::Multiplication(Box::new(lhs), Box::new(rhs)));
                }
                _ => (),
            }
            idx -= 1;
        }
        Err(InputError::ParseGeneral)
    }

    pub fn parse_value(s: &str) -> Result<Self, InputError> {
        Ok(Self::Value(s.parse::<u64>()?))
    }
}

impl FromStr for Expression {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        // parsing right-to-left, because an expression like
        // 1 + 2 * 3 + 4 is implicitly ((1 + 2) * 3) + 4

        if s.contains(|c| c == '+' || c == '*') {
            let modified = s.replace('(', " ( ").replace(')', " ) ");
            let tokens: Vec<&str> = modified.split_whitespace().collect();
            Expression::parse_expression(&tokens)
        } else {
            // no operation => should only be a value
            // remove any ()
            let modified = s.replace(|c| c == '(' || c == ')', "");
            Expression::parse_value(&modified)
        }
    }
}
