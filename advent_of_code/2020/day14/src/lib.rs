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
        Program::new()
    }
}
