use parsing::InputError;
use std::str::FromStr;

#[derive(Debug)]
pub struct Passport {
    byr: Option<String>,
    iyr: Option<String>,
    eyr: Option<String>,
    hgt: Option<String>,
    hcl: Option<String>,
    ecl: Option<String>,
    pid: Option<String>,
    cid: Option<String>,
}

impl Passport {
    pub fn new() -> Passport {
        Passport {
            byr: None,
            iyr: None,
            eyr: None,
            hgt: None,
            hcl: None,
            ecl: None,
            pid: None,
            cid: None,
        }
    }

    pub fn is_valid(&self) -> bool {
        self.byr.is_some()
            && self.iyr.is_some()
            && self.eyr.is_some()
            && self.hgt.is_some()
            && self.hcl.is_some()
            && self.ecl.is_some()
            && self.pid.is_some()
    }
}

impl Default for Passport {
    fn default() -> Self {
        Self::new()
    }
}

impl FromStr for Passport {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        // FIXME: May use From/TryFrom with a String parameter => no new allocations
        let mut pp = Passport::new();
        let tokens: Vec<&str> = s.split_whitespace().collect();
        for token in tokens.iter() {
            if let Some((field, val)) = token.split_once(':') {
                if field == "byr" {
                    pp.byr = Some(val.to_owned());
                } else if field == "iyr" {
                    pp.iyr = Some(val.to_owned());
                } else if field == "eyr" {
                    pp.eyr = Some(val.to_owned());
                } else if field == "hgt" {
                    pp.hgt = Some(val.to_owned());
                } else if field == "hcl" {
                    pp.hcl = Some(val.to_owned());
                } else if field == "ecl" {
                    pp.ecl = Some(val.to_owned());
                } else if field == "pid" {
                    pp.pid = Some(val.to_owned());
                } else if field == "cid" {
                    pp.cid = Some(val.to_owned());
                }
            }
        }
        Ok(pp)
    }
}
