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
    fn pair_iter(&self) -> PolymerIterator {
        PolymerIterator {
            index: 0,
            poly: self,
        }
    }

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

pub struct PolymerIterator<'a> {
    index: usize,
    poly: &'a Polymer,
}

impl<'a> Iterator for PolymerIterator<'a> {
    type Item = Pair;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index < self.poly.data.len() - 1 {
            let l = self.poly.data[self.index];
            self.index += 1;
            let r = self.poly.data[self.index];
            Some((l, r))
        } else {
            None
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
        for (i, p) in self.polymer.pair_iter().enumerate() {
            if let Some(rule) = self.rule(&p) {
                insertions.push((i + 1 + insertions.len(), rule.apply()));
            }
        }
        for ins in insertions {
            self.polymer.data.insert(ins.0, ins.1);
        }
    }

    pub fn polymer(&self) -> &Polymer {
        &self.polymer
    }
}

pub struct FastPolymerisation {
    // FIXME replace with a Vec by using a "hash" as index
    pair_dictionary: HashMap<(char, char, char), usize>,
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
        for i in 0..len - 2 {
            let a = polymer.data[i];
            let b = polymer.data[i + 1];
            let c = polymer.data[i + 2];
            pair_dictionary
                .entry((a, b, c))
                .and_modify(|val| *val += 1)
                .or_insert(1);

            counts[Self::to_index(a)] += 1;
        }
        // Last two chars
        let a = polymer.data[len - 2];
        let b = polymer.data[len - 1];
        counts[Self::to_index(a)] += 1;
        counts[Self::to_index(b)] += 1;
        pair_dictionary.insert((a, b, '_'), 1);

        FastPolymerisation {
            pair_dictionary,
            rules,
            counts,
        }
    }

    pub fn step(&mut self) {
        let mut new_pairs = HashMap::new();
        for (&(x, y, z), &occ) in self.pair_dictionary.iter() {
            // Apply rule
            let a = self
                .rule(&(x, y))
                .map(|r| r.apply())
                .expect("No rule found!");
            let b = self.rule(&(y, z)).map(|r| r.apply()).unwrap_or('_');

            // Update character count.
            self.counts[Self::to_index(a)] += occ;

            // Update pair counts.
            new_pairs
                .entry((x, a, y))
                .and_modify(|val| *val += occ)
                .or_insert(occ);
            new_pairs
                .entry((a, y, b))
                .and_modify(|val| *val += occ)
                .or_insert(occ);
        }
        self.pair_dictionary = new_pairs;
    }

    pub fn occurrences(&self) -> Vec<(char, usize)> {
        self.counts
            .iter()
            .enumerate()
            .filter_map(|(idx, &count)| {
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
