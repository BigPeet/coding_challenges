use parsing::InputError;
use std::fmt::Display;
use std::str::FromStr;

#[derive(Debug, Hash, Clone, Copy, PartialEq, Eq)]
struct RangeInclusive {
    start: u64,
    end: u64,
}

impl RangeInclusive {
    pub fn contains(&self, value: u64) -> bool {
        value >= self.start && value <= self.end
    }
}

impl FromStr for RangeInclusive {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        // s := x-y, e.g. 50-688
        if let Some((lower, higher)) = s.split_once('-') {
            let start = lower.trim().parse::<u64>()?;
            let end = higher.trim().parse::<u64>()?;
            return Ok(RangeInclusive { start, end });
        }
        Err(InputError::ParseGeneral)
    }
}

#[derive(Debug, Hash, PartialEq, Eq)]
pub struct TicketField {
    name: String,
    value_ranges: [RangeInclusive; 2],
}

impl Display for TicketField {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}", self.name)
    }
}

impl TicketField {
    pub fn accepts(&self, value: u64) -> bool {
        self.value_ranges[0].contains(value) || self.value_ranges[1].contains(value)
    }

    pub fn name(&self) -> &str {
        &self.name
    }
}

impl FromStr for TicketField {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((left, right)) = s.split_once(':') {
            if let Some((lower, higher)) = right.split_once(" or ") {
                let lower_r = lower.parse::<RangeInclusive>()?;
                let higher_r = higher.parse::<RangeInclusive>()?;
                return Ok(TicketField {
                    name: left.to_string(),
                    value_ranges: [lower_r, higher_r],
                });
            }
        }
        Err(InputError::ParseGeneral)
    }
}

pub struct Ticket {
    pub positions: Vec<u64>,
}

impl FromStr for Ticket {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let positions = s
            .split(',')
            .filter_map(|tok| tok.parse::<u64>().ok())
            .collect();
        Ok(Ticket { positions })
    }
}

impl Ticket {
    pub fn is_valid(&self, fields: &[TicketField]) -> bool {
        self.positions
            .iter()
            .all(|pos| fields.iter().any(|f| f.accepts(*pos)))
    }

    pub fn get_invalid(&self, fields: &[TicketField]) -> Option<&u64> {
        self.positions
            .iter()
            .find(|pos| fields.iter().all(|f| !f.accepts(**pos)))
    }
}
