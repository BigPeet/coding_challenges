use parsing::InputError;
use std::str::FromStr;

#[derive(Debug)]
pub struct PassportP1 {
    byr: bool,
    iyr: bool,
    eyr: bool,
    hgt: bool,
    hcl: bool,
    ecl: bool,
    pid: bool,
    cid: bool,
}

#[derive(Debug)]
enum Height {
    InInch(usize),
    InCm(usize),
}

#[derive(Debug)]
pub struct PassportP2 {
    byr: Option<usize>,
    iyr: Option<usize>,
    eyr: Option<usize>,
    hgt: Option<Height>,
    hcl: Option<String>,
    ecl: Option<String>,
    pid: Option<String>,
    cid: Option<String>,
}

impl FromStr for Height {
    type Err = InputError;
    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let (value, unit): (Vec<char>, Vec<char>) = s.chars().partition(|c| c.is_ascii_digit());
        let value: String = value.into_iter().collect();
        if !value.is_empty() {
            let unit: String = unit.into_iter().collect();
            if unit == "cm" {
                return Ok(Height::InCm(value.parse::<usize>()?));
            } else if unit == "in" {
                return Ok(Height::InInch(value.parse::<usize>()?));
            }
        }

        Err(Self::Err::ParseGeneral)
    }
}

impl PassportP1 {
    pub fn new() -> PassportP1 {
        PassportP1 {
            byr: false,
            iyr: false,
            eyr: false,
            hgt: false,
            hcl: false,
            ecl: false,
            pid: false,
            cid: false,
        }
    }

    pub fn is_valid(&self) -> bool {
        self.byr && self.iyr && self.eyr && self.hgt && self.hcl && self.ecl && self.pid
    }
}

impl Default for PassportP1 {
    fn default() -> Self {
        Self::new()
    }
}

impl FromStr for PassportP1 {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut pp = PassportP1::new();
        let tokens: Vec<&str> = s.split_whitespace().collect();
        for token in tokens.iter() {
            if let Some((field, _)) = token.split_once(':') {
                if field == "byr" {
                    pp.byr = true;
                } else if field == "iyr" {
                    pp.iyr = true;
                } else if field == "eyr" {
                    pp.eyr = true;
                } else if field == "hgt" {
                    pp.hgt = true;
                } else if field == "hcl" {
                    pp.hcl = true;
                } else if field == "ecl" {
                    pp.ecl = true;
                } else if field == "pid" {
                    pp.pid = true;
                } else if field == "cid" {
                    pp.cid = true;
                }
            }
        }
        Ok(pp)
    }
}

impl PassportP2 {
    pub fn new() -> PassportP2 {
        PassportP2 {
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

    fn has_valid_byr(&self) -> bool {
        if let Some(byr) = &self.byr {
            return (1920..=2002).contains(byr);
        }
        false
    }

    fn has_valid_iyr(&self) -> bool {
        if let Some(iyr) = &self.iyr {
            return (2010..=2020).contains(iyr);
        }
        false
    }

    fn has_valid_eyr(&self) -> bool {
        if let Some(eyr) = &self.eyr {
            return (2020..=2030).contains(eyr);
        }
        false
    }

    fn has_valid_hgt(&self) -> bool {
        if let Some(hgt) = &self.hgt {
            match hgt {
                Height::InCm(val) => return (150..=193).contains(val),
                Height::InInch(val) => return (50..=76).contains(val),
            }
        }
        false
    }

    fn has_valid_hcl(&self) -> bool {
        if let Some(hcl) = &self.hcl {
            if hcl.len() == 7 {
                let mut it = hcl.chars();
                if it.next().unwrap_or(' ') == '#' {
                    return it.all(|c| c.is_ascii_digit() || (('a'..='f').contains(&c)));
                }
            }
        }
        false
    }

    fn has_valid_ecl(&self) -> bool {
        if let Some(ecl) = &self.ecl {
            return ["amb", "blu", "brn", "gry", "grn", "hzl", "oth"].contains(&ecl.as_str());
        }
        false
    }

    fn has_valid_pid(&self) -> bool {
        if let Some(pid) = &self.pid {
            return pid.len() == 9 && pid.chars().all(|c| c.is_ascii_digit());
        }
        false
    }

    pub fn is_valid(&self) -> bool {
        self.has_valid_byr()
            && self.has_valid_iyr()
            && self.has_valid_eyr()
            && self.has_valid_hgt()
            && self.has_valid_hcl()
            && self.has_valid_ecl()
            && self.has_valid_pid()
    }
}

impl Default for PassportP2 {
    fn default() -> Self {
        Self::new()
    }
}

impl FromStr for PassportP2 {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        // FIXME: May use From/TryFrom with a String parameter => no new allocations
        let mut pp = PassportP2::new();
        let tokens: Vec<&str> = s.split_whitespace().collect();
        for token in tokens.iter() {
            if let Some((field, val)) = token.split_once(':') {
                if field == "byr" {
                    pp.byr = val.parse::<usize>().ok();
                } else if field == "iyr" {
                    pp.iyr = val.parse::<usize>().ok();
                } else if field == "eyr" {
                    pp.eyr = val.parse::<usize>().ok();
                } else if field == "hgt" {
                    pp.hgt = val.parse::<Height>().ok();
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
