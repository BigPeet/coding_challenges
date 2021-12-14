use parsing::InputError;
use std::collections::HashMap;
use std::fmt::Display;
use std::str::FromStr;

type Pair = (char, char);

#[derive(Clone)]
pub struct Polymer {
    data: Vec<char>,
}

impl Polymer {
    pub fn len(&self) -> usize {
        self.data.len()
    }

    pub fn is_empty(&self) -> bool {
        self.data.is_empty()
    }

    pub fn occurrences(&self) -> Vec<(char, usize)> {
        let mut counts = [0; 26];
        for c in self.data.iter() {
            let index = (*c as u8 - b'A') as usize;
            counts[index] += 1;
        }
        counts
            .into_iter()
            .enumerate()
            .filter_map(|(idx, count)| {
                if count > 0 {
                    let c = idx as u8 + b'A';
                    Some((c as char, count))
                } else {
                    None
                }
            })
            .collect()
    }
}

impl TryFrom<String> for Polymer {
    type Error = InputError;

    fn try_from(value: String) -> Result<Self, Self::Error> {
        let mut data = Vec::with_capacity(value.trim_end().len());
        for c in value.chars() {
            if c.is_ascii_uppercase() {
                data.push(c);
            } else {
                return Err(InputError::ParseGeneral);
            }
        }
        Ok(Polymer { data })
    }
}

impl Display for Polymer {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for c in self.data.iter() {
            write!(f, "{}", c)?;
        }
        write!(f, "")
    }
}

#[derive(Clone)]
pub struct InsertionRule {
    from: Pair,
    to: char,
}

impl InsertionRule {
    pub fn apply(&self) -> char {
        self.to
    }
    pub fn matches(&self, p: &Pair) -> bool {
        self.from == *p
    }
}

impl FromStr for InsertionRule {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((l, r)) = s.split_once(" -> ") {
            // FIXME: uppercase check
            let to = r.chars().next().ok_or(InputError::ParseGeneral)?;
            let mut l_iter = l.chars();
            let a = l_iter.next().ok_or(InputError::ParseGeneral)?;
            let b = l_iter.next().ok_or(InputError::ParseGeneral)?;
            Ok(InsertionRule { from: (a, b), to })
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}

pub struct Polymerisation {
    polymer: Polymer,
    rules: Vec<InsertionRule>,
}

impl Polymerisation {
    pub fn new(polymer: Polymer, rules: Vec<InsertionRule>) -> Polymerisation {
        Polymerisation { polymer, rules }
    }

    fn rule(&self, p: &Pair) -> Option<&InsertionRule> {
        for r in self.rules.iter() {
            if r.matches(p) {
                return Some(r);
            }
        }
        None
    }

    pub fn step(&mut self) {
        let mut insertions = vec![];
        for (i, p) in self.polymer.data.windows(2).enumerate() {
            if let Some(rule) = self.rule(&(p[0], p[1])) {
                insertions.push((i + 1 + insertions.len(), rule.apply()));
            }
        }
        for ins in insertions {
            self.polymer.data.insert(ins.0, ins.1);
        }
    }

    pub fn min_max(&self) -> (usize, usize) {
        let counts = self.polymer.occurrences();
        let max = counts.iter().map(|&(_, occ)| occ).max().unwrap_or(0);
        let min = counts.iter().map(|&(_, occ)| occ).min().unwrap_or(0);
        (min, max)
    }
}

pub struct FastPolymerisation {
    // FIXME replace with a Vec by using a "hash" as index
    pair_dictionary: HashMap<Pair, usize>,
    rules: Vec<InsertionRule>,
    counts: [usize; 26],
}

impl FastPolymerisation {
    fn rule(&self, p: &Pair) -> Option<&InsertionRule> {
        for r in self.rules.iter() {
            if r.matches(p) {
                return Some(r);
            }
        }
        None
    }

    fn to_index(c: char) -> usize {
        c as usize - b'A' as usize
    }

    pub fn new(polymer: Polymer, rules: Vec<InsertionRule>) -> FastPolymerisation {
        let mut pair_dictionary = HashMap::new();
        let mut counts = [0; 26];
        let len = polymer.len();
        for w in polymer.data.windows(2) {
            pair_dictionary
                .entry((w[0], w[1]))
                .and_modify(|val| *val += 1)
                .or_insert(1);
            counts[Self::to_index(w[0])] += 1;
        }
        // Last character
        let b = polymer.data[len - 1];
        counts[Self::to_index(b)] += 1;

        FastPolymerisation {
            pair_dictionary,
            rules,
            counts,
        }
    }

    pub fn step(&mut self) {
        let mut new_pairs = HashMap::new();
        for (&(x, y), &occ) in self.pair_dictionary.iter() {
            // Apply rule
            let a = self
                .rule(&(x, y))
                .map(|r| r.apply())
                .expect("No rule found!");
            // Update character count.
            self.counts[Self::to_index(a)] += occ;

            // Update pair counts.
            new_pairs
                .entry((x, a))
                .and_modify(|val| *val += occ)
                .or_insert(occ);
            new_pairs
                .entry((a, y))
                .and_modify(|val| *val += occ)
                .or_insert(occ);
        }
        self.pair_dictionary = new_pairs;
    }

    pub fn min_max(&self) -> (usize, usize) {
        let max = *self.counts.iter().max().unwrap_or(&0);
        let min = *self.counts.iter().filter(|&&c| c > 0).min().unwrap_or(&0);
        (min, max)
    }
}
