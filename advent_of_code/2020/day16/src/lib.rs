use parsing::InputError;
use std::str::FromStr;

#[derive(Debug)]
struct RangeInclusive {
    start: i32,
    end: i32,
}

impl RangeInclusive {
    pub fn contains(&self, value: i32) -> bool {
        value >= self.start && value <= self.end
    }
}

#[derive(Debug)]
pub struct TicketField {
    name: String,
    value_ranges: [RangeInclusive; 2],
}

impl TicketField {
    pub fn accepts(&self, value: i32) -> bool {
        self.value_ranges[0].contains(value) || self.value_ranges[1].contains(value)
    }
}

fn parse_range(s: &str) -> Result<RangeInclusive, InputError> {
    // s := x-y, e.g. 50-688
    if let Some((lower, higher)) = s.split_once('-') {
        let start = lower.trim().parse::<i32>()?;
        let end = higher.trim().parse::<i32>()?;
        return Ok(RangeInclusive { start, end });
    }
    Err(InputError::ParseGeneral)
}

impl FromStr for TicketField {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((left, right)) = s.split_once(':') {
            if let Some((lower, higher)) = right.split_once(" or ") {
                let lower_r = parse_range(lower)?;
                let higher_r = parse_range(higher)?;
                return Ok(TicketField {
                    name: left.to_string(),
                    value_ranges: [lower_r, higher_r],
                });
            }
        }
        Err(InputError::ParseGeneral)
    }
}
