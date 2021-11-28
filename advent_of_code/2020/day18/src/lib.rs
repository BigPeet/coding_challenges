use parsing::InputError;
use std::fmt::Display;

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
}

pub enum SearchModus {
    AddPrio,
    MulPrio,
    AllEqual,
}

pub trait ExpressionParser {
    fn search_modus() -> SearchModus;

    fn parse_line(line: &str) -> Result<Expression, InputError> {
        // parsing right-to-left, because an expression like
        // 1 + 2 + 3 + 4 is implicitly ((1 + 2) + 3) + 4
        let modified = line.replace('(', " ( ").replace(')', " ) ");
        let tokens: Vec<&str> = modified.split_whitespace().collect();
        Self::parse_expression(&tokens)
    }

    fn parse_value(s: &str) -> Result<Expression, InputError> {
        Ok(Expression::Value(s.parse::<u64>()?))
    }

    fn parse_parens(tokens: &[&str]) -> Result<Expression, InputError> {
        Ok(Expression::Parens(Box::new(Self::parse_expression(
            &tokens[1..tokens.len() - 1],
        )?)))
    }

    fn parse_operation(tokens: &[&str], modus: SearchModus) -> Result<Expression, InputError> {
        let token_length = tokens.len();
        let mut parens_level = 0;
        let mut idx = token_length;
        let mut to_parse: Option<(usize, &str)> = None;

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
                    if !matches!(modus, SearchModus::MulPrio) {
                        to_parse = Some((idx, *tok));
                        break;
                    } else if to_parse.is_none() {
                        to_parse = Some((idx, *tok));
                    }
                }
                "*" if parens_level == 0 => {
                    if !matches!(modus, SearchModus::AddPrio) {
                        to_parse = Some((idx, *tok));
                        break;
                    } else if to_parse.is_none() {
                        to_parse = Some((idx, *tok));
                    }
                }
                _ => (),
            }
            idx -= 1;
        }
        if let Some((idx, tok)) = to_parse {
            let lhs = Self::parse_expression(&tokens[..idx - 1])?;
            let rhs = Self::parse_expression(&tokens[idx..])?;
            match tok {
                "*" => Ok(Expression::Multiplication(Box::new(lhs), Box::new(rhs))),
                "+" => Ok(Expression::Addition(Box::new(lhs), Box::new(rhs))),
                _ => Err(InputError::ParseGeneral),
            }
        } else {
            Err(InputError::ParseGeneral)
        }
    }

    fn parse_expression(tokens: &[&str]) -> Result<Expression, InputError> {
        let token_length = tokens.len();
        // Has the expression the form (...) where the outer parens belong together? => parse inner
        if tokens[0] == "("
            && tokens[token_length - 1] == ")"
            && Expression::find_closing_paren(tokens, 1).unwrap_or(0) == token_length - 1
        {
            return Self::parse_parens(tokens);
        }

        if !tokens.contains(&"+") && !tokens.contains(&"*") {
            // no operation => should only be a value
            // remove any ()
            let filtered: Vec<&&str> = tokens
                .iter()
                .filter(|tok| **tok != "(" && **tok != ")")
                .collect();
            return Self::parse_value(filtered[0]);
        }

        Self::parse_operation(tokens, Self::search_modus())
    }
}

pub struct NormalParser;
impl ExpressionParser for NormalParser {
    fn search_modus() -> SearchModus {
        SearchModus::AllEqual
    }
}

pub struct AdvancedParser;
impl ExpressionParser for AdvancedParser {
    fn search_modus() -> SearchModus {
        SearchModus::MulPrio
    }
}
