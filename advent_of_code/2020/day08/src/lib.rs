use parsing::InputError;
use std::fmt::Display;
use std::str::FromStr;

#[derive(Debug, Clone)]
pub enum Instruction {
    NOOP(i32),
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
                "nop" => return Ok(Instruction::NOOP(param)),
                "acc" => return Ok(Instruction::ACC(param)),
                "jmp" => return Ok(Instruction::JMP(param)),
                _ => return Err(InputError::ParseGeneral), // FIXME: better error handling
            };
        }
        Err(InputError::ParseGeneral)
    }
}

impl Display for Instruction {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match &self {
            Instruction::NOOP(param) => write!(f, "nop {:+}", param),
            Instruction::ACC(param) => write!(f, "acc {:+}", param),
            Instruction::JMP(param) => write!(f, "jmp {:+}", param),
        }
    }
}

#[derive(Clone)]
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
        if self.is_running() {
            let cur = &self.instructions[self.ic];
            match cur {
                Instruction::NOOP(_) => self.ic += 1,
                Instruction::ACC(inc) => {
                    self.acc += inc;
                    self.ic += 1;
                }
                Instruction::JMP(offset) => {
                    let new_ic = self.ic as i32 + *offset;
                    self.ic = new_ic as usize;
                }
            }
        }
        self.ic
    }

    pub fn acc_value(&self) -> i32 {
        self.acc
    }

    pub fn is_running(&self) -> bool {
        self.ic < self.instructions.len()
    }

    pub fn sucess(&self) -> bool {
        self.ic == self.instructions.len()
    }

    pub fn reset(&mut self) {
        self.ic = 0;
        self.acc = 0;
    }

    pub fn get_instruction(&self, index: usize) -> &Instruction {
        &self.instructions[index]
    }

    pub fn switch_instruction(&mut self, index: usize) {
        match self.instructions[index] {
            Instruction::JMP(offset) => self.instructions[index] = Instruction::NOOP(offset),
            Instruction::NOOP(param) => self.instructions[index] = Instruction::JMP(param),
            _ => (),
        }
    }
}
