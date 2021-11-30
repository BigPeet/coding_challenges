use parsing::InputError;
use std::str::FromStr;

#[derive(Debug, Clone)]
pub enum Rule {
    Value(usize, String),
    Forward(usize, usize),
    And(usize, Vec<usize>),
    Or(usize, Vec<usize>, Vec<usize>),
}

impl Rule {
    pub fn get_id(&self) -> usize {
        match *self {
            Self::Value(id, _) => id,
            Self::Forward(id, _) => id,
            Self::And(id, _) => id,
            Self::Or(id, _, _) => id,
        }
    }

    fn parse_char(id: usize, s: &str) -> Result<Rule, InputError> {
        let val = s
            .find('"')
            .and_then(|idx| s.chars().nth(idx + 1))
            .ok_or(InputError::ParseGeneral)?;
        Ok(Rule::Value(id, val.to_string()))
    }

    fn parse_rule_list(id: usize, s: &str) -> Result<Rule, InputError> {
        if let Ok(val) = s.trim().parse::<usize>() {
            Ok(Rule::Forward(id, val))
        } else {
            let mut forwards = vec![];
            for tok in s.trim().split_whitespace().into_iter() {
                let val = tok.parse::<usize>()?;
                forwards.push(val);
            }
            Ok(Rule::And(id, forwards))
        }
    }

    fn parse_or(id: usize, s: &str) -> Result<Rule, InputError> {
        if let Some((l, r)) = s.split_once('|') {
            let mut lefts = vec![];
            for tok in l.trim().split_whitespace().into_iter() {
                let val = tok.parse::<usize>()?;
                lefts.push(val);
            }
            let mut rights = vec![];
            for tok in r.trim().split_whitespace().into_iter() {
                let val = tok.parse::<usize>()?;
                rights.push(val);
            }

            Ok(Rule::Or(id, lefts, rights))
        } else {
            Err(InputError::ParseGeneral)
        }
    }

    pub fn matches(&self, input: &str, rules: &[Rule]) -> bool {
        let (matched, remainder) = self.match_str(input, rules);
        matched && remainder.is_empty()
    }

    fn match_and<'a>(indeces: &[usize], input: &'a str, rules: &[Rule]) -> (bool, &'a str) {
        let mut remainder = input;
        for fidx in indeces.iter() {
            if remainder.is_empty() {
                return (false, input);
            }
            let (matched, rest) = rules[*fidx].match_str(remainder, rules);
            if matched {
                remainder = rest;
            } else {
                return (false, input);
            }
        }
        (true, remainder)
    }

    fn match_str<'a>(&self, input: &'a str, rules: &[Rule]) -> (bool, &'a str) {
        match self {
            Self::Value(_id, s) => {
                if input.starts_with(s) {
                    (true, &input[s.len()..])
                } else {
                    (false, input)
                }
            }
            Self::Forward(_id, fidx) => rules[*fidx].match_str(input, rules),
            Self::And(_id, v) => Rule::match_and(v, input, rules),
            Self::Or(id, l, r) => {
                // cycle circumvention
                // (won't conflict with part 1, since 8 and 11 are not Or's)
                //
                // 1. We know the rule 0 is 8 11, rule 8 is 4 | 42 8, rule 11 is 42 31 | 42 11 31
                // 2. We know that rule 8 will match rule 42 as often as possible (at least once).
                // 3. We know that rule 11 will match an amount of 42 (at least once) and then the
                //    same amount of 31.
                // 4. Hypothesis: 42 and 31 don't "match" together (too lazy to check).
                //
                // Strategy: When encountering rule 8 from rule 0, apply rule 42 at least once and
                // then repeat until the remainder is matched by rule 11.
                if *id == 8 {
                    let mut matched = false;
                    let mut last_matched = true;
                    let mut remainder = input;
                    while last_matched {
                        let (it_matched, it_remainder) = Rule::match_and(l, remainder, rules);
                        last_matched = it_matched;
                        if last_matched {
                            remainder = it_remainder;
                            matched = true;
                            // would rule 11 match now?
                            let (eleven_match, _) = rules[11].match_str(remainder, rules);
                            if eleven_match {
                                // ok abort now
                                return (true, remainder);
                            }
                        }
                    }
                    (matched, remainder)
                } else if *id == 11 && l.len() == 2 {
                    // Apply 42 as often as possible.
                    // The check if you can apply 31 the same amount of time to each remainder.
                    // Both should be applied at least once => else false
                    let mut remainder = input;
                    let mut last_matched = true;
                    let mut remainders = vec![];
                    while last_matched {
                        let (it_matched, it_remainder) = rules[l[0]].match_str(remainder, rules);
                        last_matched = it_matched;
                        if last_matched {
                            remainders.push(it_remainder);
                            remainder = it_remainder;
                        }
                    }
                    for (reps, rem) in remainders.iter().enumerate().rev() {
                        let mut complete_match = true;
                        let mut rep_remainder = *rem;
                        for _ in 0..=reps {
                            let (it_matched, it_remainder) =
                                rules[l[1]].match_str(rep_remainder, rules);
                            if !it_matched {
                                complete_match = false;
                                break;
                            } else {
                                rep_remainder = it_remainder;
                            }
                        }
                        if complete_match {
                            return (true, rep_remainder);
                        }
                    }
                    (false, input)
                } else {
                    // Normal OR
                    let (l_matched, l_remainder) = Rule::match_and(l, input, rules);
                    if l_matched {
                        return (true, l_remainder);
                    }
                    Rule::match_and(r, input, rules)
                }
            }
        }
    }
}

impl PartialEq for Rule {
    fn eq(&self, other: &Self) -> bool {
        self.get_id() == other.get_id()
    }
}

impl Eq for Rule {}

impl PartialOrd for Rule {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        self.get_id().partial_cmp(&other.get_id())
    }
}

impl Ord for Rule {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.get_id().cmp(&other.get_id())
    }
}

impl FromStr for Rule {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((id, content)) = s.split_once(':') {
            let id = id.parse::<usize>()?;
            if content.contains('"') {
                Ok(Rule::parse_char(id, content)?)
            } else if content.contains('|') {
                Ok(Rule::parse_or(id, content)?)
            } else {
                Ok(Rule::parse_rule_list(id, content)?)
            }
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}
