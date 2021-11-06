pub struct Slope {
    right: usize,
    down: usize,
}

pub struct Counter {
    slope: Slope,
    pos: usize,
    count: usize,
    line_length: usize,
}

impl Slope {
    pub fn new(right: usize, down: usize) -> Slope {
        Slope { right, down }
    }
}

impl Counter {
    const TREE: u8 = b'#';

    pub fn new(slope: Slope, line_length: usize) -> Counter {
        Counter {
            slope,
            pos: 0,
            count: 0,
            line_length,
        }
    }

    pub fn update(&mut self, line: &str, line_no: usize) {
        if line_no % self.slope.down == 0 {
            if line.as_bytes()[self.pos] == Self::TREE {
                self.count += 1;
            }
            self.pos = (self.pos + self.slope.right) % self.line_length;
        }
    }

    pub fn count(&self) -> usize {
        self.count
    }
}
