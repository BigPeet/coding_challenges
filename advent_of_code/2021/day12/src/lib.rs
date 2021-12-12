use parsing::InputError;
use std::collections::HashMap;
use std::collections::VecDeque;
use std::fmt::Display;
use std::rc::Rc;
use std::str::FromStr;

#[derive(Debug, Clone, PartialEq, Eq, Hash)]
pub struct Cave {
    name: String,
    is_small: bool,
}

impl Cave {
    pub fn new(name: &str) -> Self {
        let is_small = name.chars().all(|c| c.is_lowercase());
        Cave {
            name: name.to_string(),
            is_small,
        }
    }
}

#[derive(Debug)]
struct CaveConnections {
    map: HashMap<Rc<Cave>, Vec<Rc<Cave>>>,
}

impl CaveConnections {
    fn new() -> Self {
        CaveConnections {
            map: HashMap::new(),
        }
    }

    fn add_connection(&mut self, a: Cave, b: Cave) {
        let a = Rc::new(a);
        let b = Rc::new(b);
        self.map
            .entry(Rc::clone(&a))
            .and_modify(|exits| exits.push(Rc::clone(&b)))
            .or_insert_with(|| vec![Rc::clone(&b)]);
        self.map
            .entry(Rc::clone(&b))
            .and_modify(|exits| exits.push(Rc::clone(&a)))
            .or_insert_with(|| vec![Rc::clone(&a)]);
    }
}

#[derive(Debug)]
pub struct CaveSystem {
    connections: CaveConnections,
}

impl FromStr for CaveSystem {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut connections = CaveConnections::new();
        for line in s.lines() {
            if let Some((l, r)) = line.trim_end().split_once('-') {
                connections.add_connection(Cave::new(l), Cave::new(r));
            } else {
                return Err(InputError::ParseGeneral);
            }
        }
        Ok(Self { connections })
    }
}

impl CaveSystem {
    fn get_start(&self) -> Option<&Cave> {
        if let Some(rc_cave) = self.connections.map.keys().find(|c| c.name == "start") {
            Some(rc_cave)
        } else {
            None
        }
    }

    pub fn normal_path_filter(cave: &Cave, path: &Path) -> bool {
        !cave.is_small || !path.contains(cave)
    }

    pub fn advanced_path_filter(cave: &Cave, path: &Path) -> bool {
        !cave.is_small
            || (cave.name != "start" && (!path.contains(cave) || !path.visits_small_cave_twice()))
    }

    pub fn find_all_paths(&self, filter: fn(&Cave, &Path) -> bool) -> Vec<Path> {
        let mut found_paths = vec![];
        let mut queue = VecDeque::new();
        if let Some(start) = self.get_start() {
            queue.push_back((start, Path::new()));
        }

        while let Some((cave, mut path)) = queue.pop_front() {
            path.add(cave);
            if cave.name == "end" {
                found_paths.push(path);
            } else if let Some(connections) = self.connections.map.get(cave) {
                for valid_target in connections.iter().filter(|cave| filter(cave, &path)) {
                    queue.push_back((valid_target, path.clone()));
                }
            }
        }
        found_paths
    }
}

#[derive(Debug, Clone)]
pub struct Path<'a> {
    caves: Vec<&'a Cave>,
    small_cave_visited_twice: bool,
}

impl<'a> Path<'a> {
    pub fn contains(&self, cave: &Cave) -> bool {
        self.caves.contains(&cave)
    }

    pub fn visits_small_cave_twice(&self) -> bool {
        self.small_cave_visited_twice
    }

    pub fn add(&mut self, cave: &'a Cave) {
        if !self.small_cave_visited_twice && cave.is_small && self.caves.contains(&cave) {
            self.small_cave_visited_twice = true;
        }
        self.caves.push(cave);
    }

    pub fn new() -> Self {
        Path {
            caves: Vec::new(),
            small_cave_visited_twice: false,
        }
    }
}

impl<'a> Display for Path<'a> {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for c in self.caves.iter().take(self.caves.len() - 1) {
            write!(f, "{},", c.name)?;
        }
        write!(f, "{}", self.caves[self.caves.len() - 1].name)
    }
}

impl<'a> Default for Path<'a> {
    fn default() -> Self {
        Self::new()
    }
}
