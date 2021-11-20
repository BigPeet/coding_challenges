use parsing::InputError;
use std::str::FromStr;

#[derive(Debug)]
pub enum Instruction {
    NOOP,
    ACC(i32),
    JMP(i32),
}

impl FromStr for Instruction {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        // s := CMD [+|-]NUM
        if let Some((cmd, num)) = s.split_once(' ') {
            let param = num.parse::<i32>()?;
            match cmd {
                "nop" => return Ok(Instruction::NOOP),
                "acc" => return Ok(Instruction::ACC(param)),
                "jmp" => return Ok(Instruction::JMP(param)),
                _ => return Err(InputError::ParseGeneral), // FIXME: better error handling
            };
        }
        Err(InputError::ParseGeneral)
    }
}

pub struct Program {
    instructions: Vec<Instruction>,
    acc: i32,
    ic: usize,
}

impl Program {
    pub fn new(instructions: Vec<Instruction>) -> Program {
        Program {
            instructions,
            acc: 0,
            ic: 0,
        }
    }

    pub fn step(&mut self) -> usize {
        let cur = &self.instructions[self.ic];
        match cur {
            Instruction::NOOP => self.ic += 1,
            Instruction::ACC(inc) => {
                self.acc += inc;
                self.ic += 1;
            }
            Instruction::JMP(offset) => {
                let new_ic = self.ic as i32 + *offset;
                self.ic = new_ic as usize;
            }
        }
        self.ic
    }

    pub fn acc_value(&self) -> i32 {
        self.acc
    }
}
