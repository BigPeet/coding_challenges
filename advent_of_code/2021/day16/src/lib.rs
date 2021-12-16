use parsing::InputError;

type Bit = u8;
type Literal = u64;

#[derive(Debug)]
pub enum Package {
    LiteralPackge {
        info: PackageInfo,
        literal: Literal,
    },
    OperatorPackage {
        info: PackageInfo,
        subpackages: Vec<Package>,
    },
}

#[derive(Debug)]
pub struct PackageInfo {
    version_id: u8,
    type_id: u8,
}

impl Package {
    pub fn version_check_sum(&self) -> u32 {
        match self {
            Package::LiteralPackge { info, .. } => info.version_id as u32,
            Package::OperatorPackage { info, subpackages } => {
                subpackages.iter().fold(info.version_id as u32, |acc, sub| {
                    acc + sub.version_check_sum()
                })
            }
        }
    }

    pub fn evaluate(&self) -> Result<Literal, InputError> {
        match self {
            Package::LiteralPackge { literal, .. } => Ok(*literal),
            Package::OperatorPackage { info, subpackages } => match info.type_id {
                0 => subpackages
                    .iter()
                    .fold(Ok(0), |acc, sub| Ok(acc? + sub.evaluate()?)),
                1 => subpackages
                    .iter()
                    .fold(Ok(1), |acc, sub| Ok(acc? * sub.evaluate()?)),
                2 => subpackages.iter().fold(Ok(Literal::MAX), |acc, sub| {
                    acc.and_then(|val| {
                        let sub_val = sub.evaluate()?;
                        Ok(sub_val.min(val))
                    })
                }),
                3 => subpackages.iter().fold(Ok(Literal::MIN), |acc, sub| {
                    acc.and_then(|val| {
                        let sub_val = sub.evaluate()?;
                        Ok(sub_val.max(val))
                    })
                }),
                5 => {
                    if subpackages.len() == 2 {
                        let a = subpackages[0].evaluate()?;
                        let b = subpackages[1].evaluate()?;
                        if a > b {
                            Ok(1)
                        } else {
                            Ok(0)
                        }
                    } else {
                        Err(InputError::ParseGeneral)
                    }
                }
                6 => {
                    if subpackages.len() == 2 {
                        let a = subpackages[0].evaluate()?;
                        let b = subpackages[1].evaluate()?;
                        if a < b {
                            Ok(1)
                        } else {
                            Ok(0)
                        }
                    } else {
                        Err(InputError::ParseGeneral)
                    }
                }
                7 => {
                    if subpackages.len() == 2 {
                        let a = subpackages[0].evaluate()?;
                        let b = subpackages[1].evaluate()?;
                        if a == b {
                            Ok(1)
                        } else {
                            Ok(0)
                        }
                    } else {
                        Err(InputError::ParseGeneral)
                    }
                }
                _ => Err(InputError::ParseGeneral),
            },
        }
    }
}

fn char_to_bits(c: char) -> Result<[Bit; 4], InputError> {
    let x = c.to_digit(16).ok_or(InputError::ParseGeneral)? as Bit;
    Ok([x >> 3, (x >> 2) & 1, (x >> 1) & 1, x & 1])
}

pub fn str_to_bits(hex_string: &str) -> Result<Vec<Bit>, InputError> {
    let trimmed = hex_string.trim_end();
    let mut bits = Vec::with_capacity(trimmed.len() * 4);
    for c in trimmed.chars() {
        bits.extend(char_to_bits(c)?);
    }
    Ok(bits)
}

fn to_id(bits: &[Bit]) -> Result<u8, InputError> {
    if bits.len() >= 3 {
        Ok((bits[0] << 2) + (bits[1] << 1) + bits[2])
    } else {
        Err(InputError::ParseGeneral)
    }
}

fn to_literal(bits: &[Bit]) -> Literal {
    let mut literal: Literal = 0;
    for (n, &bit) in bits.iter().rev().enumerate() {
        literal += (bit as Literal) << n;
    }
    literal
}

fn try_parse_literal(bits: &[Bit]) -> Result<(Literal, usize), InputError> {
    let mut remaining_bits = bits;
    let mut literal_bits: Vec<Bit> = Vec::new();
    let mut end = 0;
    while remaining_bits.len() >= 5 {
        end += 5;
        let prefix = remaining_bits[0];
        literal_bits.extend(&remaining_bits[1..5]);
        if prefix == 1 {
            // FIXME: len check needed?
            // another round
            remaining_bits = &remaining_bits[5..];
        } else if prefix == 0 {
            // this was the last round
            let literal = to_literal(&literal_bits);
            return Ok((literal, end));
        }
    }
    Err(InputError::ParseGeneral)
}

fn try_subpackages_from(bits: &[Bit]) -> Result<(Vec<Package>, usize), InputError> {
    let length_type_id = bits[0];
    if length_type_id == 0 && bits.len() >= 16 {
        // next 15 bits are number of subpackage bits.
        let num_of_bits = to_literal(&bits[1..16]) as usize;
        if bits.len() >= num_of_bits + 16 {
            let sub_bits = &bits[16..(16 + num_of_bits)];
            let mut start = 0;
            let mut subs = Vec::new();
            while start < num_of_bits {
                let (sub, end) = try_package_from(&sub_bits[start..])?;
                start += end;
                subs.push(sub);
            }
            return Ok((subs, start + 16));
        }
    } else if length_type_id == 1 && bits.len() >= 12 {
        // next 11 bits is number of subpackages.
        let num_of_subs = to_literal(&bits[1..12]) as usize;
        let mut subs = Vec::with_capacity(num_of_subs);
        let mut start = 0;
        let sub_bits = &bits[12..];
        for _ in 0..num_of_subs {
            let (sub, end) = try_package_from(&sub_bits[start..])?;
            start += end;
            subs.push(sub);
        }
        return Ok((subs, start + 12));
    }
    Err(InputError::ParseGeneral)
}

const HEADER_LEN: usize = 6;

pub fn try_package_from(bits: &[Bit]) -> Result<(Package, usize), InputError> {
    let len = bits.len();
    if len > HEADER_LEN {
        let version_id = to_id(&bits[0..3])?;
        let type_id = to_id(&bits[3..6])?;
        let info = PackageInfo {
            version_id,
            type_id,
        };
        let remaining_bits = &bits[HEADER_LEN..];
        match type_id {
            4 => {
                let (literal, end) = try_parse_literal(remaining_bits)?;
                Ok((Package::LiteralPackge { info, literal }, end + HEADER_LEN))
            }
            _ => {
                let (subpackages, end) = try_subpackages_from(remaining_bits)?;
                Ok((
                    Package::OperatorPackage { info, subpackages },
                    end + HEADER_LEN,
                ))
            }
        }
    } else {
        Err(InputError::ParseGeneral)
    }
}
