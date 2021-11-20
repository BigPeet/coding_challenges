use parsing::InputError;
use std::fmt::Display;
use std::str::FromStr;

#[derive(Debug, PartialEq, Eq, Clone)]
pub struct Bag {
    adj: String,
    color: String,
}

impl Bag {
    pub fn new(adj: &str, color: &str) -> Bag {
        Bag {
            adj: adj.to_string(),
            color: color.to_string(),
        }
    }
}

impl Display for Bag {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "'{} {} bag'", self.adj, self.color)
    }
}

impl FromStr for Bag {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        // assuming s of the form 'adjective color'
        if let Some((adj, color)) = s.split_once(' ') {
            let adj = adj.trim_end().to_owned();
            let color = color.trim_end().to_owned();
            return Ok(Bag { adj, color });
        }
        Err(InputError::ParseGeneral)
    }
}

#[derive(Debug)]
pub struct Content {
    pub amount: usize,
    pub bag: Bag,
}

impl FromStr for Content {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        // assuming s of the form 'NUM ADJ COLOR bag(s)?[,.]
        let amount_end = s.find(' ').ok_or(InputError::ParseGeneral)?;
        let amount = s[..amount_end].parse::<usize>()?;

        let contained_end = s[amount_end + 1..]
            .find("bag")
            .ok_or(InputError::ParseGeneral)?
            + amount_end
            + 1;
        let contained = s[amount_end + 1..contained_end].parse::<Bag>()?;

        Ok(Content {
            amount,
            bag: contained,
        })
    }
}

#[derive(Debug)]
pub struct Rule {
    container: Bag,
    contents: Vec<Content>,
}

impl Rule {
    pub fn allows(&self, bag: &Bag) -> bool {
        self.contents.iter().any(|c| c.bag == *bag)
    }

    pub fn container(&self) -> &Bag {
        &self.container
    }

    pub fn contents(&self) -> &[Content] {
        &self.contents
    }
}

impl FromStr for Rule {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        // assuming s of the form 'ADJ COLOR bags contain (NUM ADJ COLOR bags,)*NUM ADJ COLOR bags.'
        let splitter = " bags contain ";
        let container_end = s.find(splitter).ok_or(InputError::ParseGeneral)?;
        let container = s[..container_end].parse::<Bag>()?;

        let mut content_idx = container_end + splitter.len();
        let mut contents = Vec::new();

        if !s[content_idx..].contains("no other bags.") {
            let splitter = ", ";
            while let Some(end_idx) = s[content_idx..].find(splitter) {
                let end_idx = content_idx + end_idx;
                let content = s[content_idx..end_idx].parse::<Content>()?;
                contents.push(content);
                content_idx = end_idx + splitter.len();
            }

            if let Some(end_idx) = s[content_idx..].find('.') {
                let end_idx = content_idx + end_idx;
                let content = s[content_idx..end_idx].parse::<Content>()?;
                contents.push(content);
            }
        }

        Ok(Rule {
            container,
            contents,
        })
    }
}
