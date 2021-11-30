use parsing::InputError;
use std::str::FromStr;

#[derive(Debug, Clone)]
pub enum RuleKind {
    Value(String),
    Forward(usize),
    And(Vec<RuleKind>),
    Or(Box<RuleKind>, Box<RuleKind>),
}

impl RuleKind {
    fn parse_char(s: &str) -> Result<RuleKind, InputError> {
        let val = s
            .find('"')
            .and_then(|idx| s.chars().nth(idx + 1))
            .ok_or(InputError::ParseGeneral)?;
        Ok(RuleKind::Value(val.to_string()))
    }

    fn parse_rule_list(s: &str) -> Result<RuleKind, InputError> {
        if let Ok(val) = s.trim().parse::<usize>() {
            Ok(RuleKind::Forward(val))
        } else {
            let mut forwards = vec![];
            for tok in s.trim().split_whitespace().into_iter() {
                let val = RuleKind::Forward(tok.parse::<usize>()?);
                forwards.push(val);
            }
            Ok(RuleKind::And(forwards))
        }
    }

    fn parse_or(s: &str) -> Result<RuleKind, InputError> {
        if let Some((l, r)) = s.split_once('|') {
            Ok(RuleKind::Or(
                Box::new(Self::parse_rule_list(l)?),
                Box::new(Self::parse_rule_list(r)?),
            ))
        } else {
            Err(InputError::ParseGeneral)
        }
    }

    pub fn matches(&self, input: &str, rules: &[Rule]) -> bool {
        let (matched, remainder) = self.match_str(input, rules);
        matched && remainder.is_empty()
    }

    fn match_str<'a>(&self, input: &'a str, rules: &[Rule]) -> (bool, &'a str) {
        match self {
            Self::Value(s) => {
                if input.starts_with(s) {
                    (true, &input[s.len()..])
                } else {
                    (false, input)
                }
            }
            Self::Forward(fidx) => rules[*fidx].rule.match_str(input, rules),
            Self::And(v) => {
                let mut remainder = input;
                for rk in v.iter() {
                    let (matched, rest) = rk.match_str(remainder, rules);
                    if matched {
                        remainder = rest;
                    } else {
                        return (false, input);
                    }
                }
                (true, remainder)
            }
            Self::Or(l, r) => {
                let (matched, remainder) = l.match_str(input, rules);
                if !matched {
                    return r.match_str(input, rules);
                }
                (matched, remainder)
            }
        }
    }
}

#[derive(Debug)]
pub struct Rule {
    id: usize,
    pub rule: RuleKind,
}

impl PartialEq for Rule {
    fn eq(&self, other: &Self) -> bool {
        self.id == other.id
    }
}

impl Eq for Rule {}

impl PartialOrd for Rule {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        self.id.partial_cmp(&other.id)
    }
}

impl Ord for Rule {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.id.cmp(&other.id)
    }
}

impl FromStr for Rule {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((id, content)) = s.split_once(':') {
            let id = id.parse::<usize>()?;
            if content.contains('"') {
                Ok(Rule {
                    id,
                    rule: RuleKind::parse_char(content)?,
                })
            } else if content.contains('|') {
                Ok(Rule {
                    id,
                    rule: RuleKind::parse_or(content)?,
                })
            } else {
                Ok(Rule {
                    id,
                    rule: RuleKind::parse_rule_list(content)?,
                })
            }
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}
