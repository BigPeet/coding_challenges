use parsing::InputError;
use std::collections::HashMap;
use std::str::FromStr;

#[derive(Debug, Clone, Copy)]
pub struct Mask {
    ones: u64,
    zeros: u64,
}

#[derive(Debug, Clone, Copy)]
pub enum Operation {
    MaskOp(Mask),
    WriteOp { address: u64, value: u64 },
}

impl FromStr for Operation {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, <Self as FromStr>::Err> {
        let splits = s.split_once('=').ok_or(InputError::ParseGeneral)?;

        if splits.0.starts_with("mask") {
            let ones = splits
                .1
                .trim()
                .chars()
                .rev()
                .map(|c| if c == '1' { 1u64 } else { 0u64 })
                .enumerate()
                .fold(0u64, |acc, (i, val)| acc | (val << i));
            let zeros = splits
                .1
                .trim()
                .chars()
                .rev()
                .map(|c| if c == '0' { 0u64 } else { 1u64 })
                .enumerate()
                .fold(0u64, |acc, (i, val)| acc | (val << i));
            return Ok(Operation::MaskOp(Mask { ones, zeros }));
        } else if splits.0.starts_with("mem[") {
            let end_idx = splits.0.find(']').ok_or(InputError::ParseGeneral)?;
            let address = splits.0[4..end_idx].parse::<u64>()?;
            let value = splits.1.trim().parse::<u64>()?;
            return Ok(Operation::WriteOp { address, value });
        }

        Err(InputError::ParseGeneral)
    }
}

#[derive(Debug, Clone, Copy)]
pub enum OpChar {
    One,
    Zero,
    X,
}

#[derive(Debug, Clone, Copy)]
pub enum OperationV2 {
    MaskOp([OpChar; 36]),
    WriteOp { address: u64, value: u64 },
}

impl FromStr for OperationV2 {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, <Self as FromStr>::Err> {
        let splits = s.split_once('=').ok_or(InputError::ParseGeneral)?;

        if splits.0.starts_with("mask") {
            let mut mask_chars: [OpChar; 36] = [OpChar::Zero; 36];
            for (i, c) in splits.1.trim().chars().rev().enumerate() {
                if i >= 36 {
                    return Err(InputError::ParseGeneral);
                }
                mask_chars[i] = match c {
                    '1' => OpChar::One,
                    '0' => OpChar::Zero,
                    _ => OpChar::X,
                };
            }
            return Ok(OperationV2::MaskOp(mask_chars));
        } else if splits.0.starts_with("mem[") {
            let end_idx = splits.0.find(']').ok_or(InputError::ParseGeneral)?;
            let address = splits.0[4..end_idx].parse::<u64>()?;
            let value = splits.1.trim().parse::<u64>()?;
            return Ok(OperationV2::WriteOp { address, value });
        }
        Err(InputError::ParseGeneral)
    }
}

#[derive(Debug)]
pub struct Program {
    mask: Mask,
    memory: HashMap<u64, u64>,
}

impl Program {
    pub fn new() -> Self {
        Self {
            mask: Mask { ones: 0, zeros: 0 },
            memory: HashMap::new(),
        }
    }

    fn write(&mut self, address: u64, value: u64) {
        let masked_value = (value | self.mask.ones) & self.mask.zeros;
        self.memory.insert(address, masked_value);
    }

    pub fn apply(&mut self, op: Operation) {
        match op {
            Operation::MaskOp(mask) => self.mask = mask,
            Operation::WriteOp { address, value } => self.write(address, value),
        }
    }

    pub fn sum(&self) -> u64 {
        self.memory.values().sum()
    }
}

impl Default for Program {
    fn default() -> Self {
        Self::new()
    }
}

#[derive(Debug)]
pub struct ProgramV2 {
    mask: [OpChar; 36],
    memory: HashMap<u64, u64>,
}

impl ProgramV2 {
    pub fn new() -> Self {
        Self {
            mask: [OpChar::Zero; 36],
            memory: HashMap::new(),
        }
    }

    fn generate_addresses(&self, address: u64) -> Vec<u64> {
        let mut addresses = Vec::new();
        let or_mask = self
            .mask
            .iter()
            .map(|opc| match *opc {
                OpChar::One => 1u64,
                _ => 0u64,
            })
            .enumerate()
            .fold(0, |acc, (i, num)| acc | (num << i));

        let masked_address = address | or_mask;
        let x_indeces: Vec<usize> = self
            .mask
            .iter()
            .enumerate()
            .filter_map(|(i, opc)| match *opc {
                OpChar::X => Some(i),
                _ => None,
            })
            .collect();

        // Generate all possible combinations for X's.
        for mask_value in 0..2u64.pow(x_indeces.len() as u32) {
            let mut generated = masked_address;
            for (i, x_idx) in x_indeces.iter().enumerate() {
                // Get the value for this X in this combination.
                let x_mask = (mask_value >> i) & 1u64;
                if x_mask == 1 {
                    // set bit at X's index to 1
                    generated |= 1 << x_idx;
                } else if x_mask == 0 {
                    // set bit at X's index to 0
                    generated &= !(1 << x_idx);
                }
            }
            addresses.push(generated);
        }

        addresses
    }

    fn write(&mut self, address: u64, value: u64) {
        self.generate_addresses(address).iter().for_each(|add| {
            self.memory.insert(*add, value);
        });
    }

    pub fn apply(&mut self, op: OperationV2) {
        match op {
            OperationV2::MaskOp(mask) => self.mask = mask,
            OperationV2::WriteOp { address, value } => self.write(address, value),
        }
    }

    pub fn sum(&self) -> u64 {
        self.memory.values().sum()
    }
}

impl Default for ProgramV2 {
    fn default() -> Self {
        Self::new()
    }
}
