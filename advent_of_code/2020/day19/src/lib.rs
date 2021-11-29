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

    pub fn evaluate(&self, rules: &[Rule]) -> Vec<String> {
        let ret = match self {
            Self::Value(s) => vec![s.clone()],
            Self::Forward(fidx) => rules[*fidx].rule.evaluate(rules),
            Self::And(v) => {
                let mut all_results: Vec<Vec<String>> = vec![];
                for rk in v.iter() {
                    all_results.push(rk.evaluate(rules));
                }

                let mut combined = vec!["".to_owned()];
                let mut helper = vec![];
                for results in all_results.iter() {
                    for entry in combined.iter() {
                        for res in results.iter() {
                            helper.push(entry.clone() + res.as_str());
                        }
                    }
                    combined = helper;
                    helper = vec![];
                }
                combined
            }
            Self::Or(l, r) => {
                let mut combined = l.evaluate(rules);
                combined.extend(r.evaluate(rules));
                combined
            }
        };
        ret
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
