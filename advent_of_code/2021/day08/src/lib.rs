use parsing::InputError;
use std::str::FromStr;

const TOP: usize = 0;
const TOP_LEFT: usize = 1;
const TOP_RIGHT: usize = 2;
const MIDDLE: usize = 3;
const BOTTOM_LEFT: usize = 4;
const BOTTOM_RIGHT: usize = 5;
const BOTTOM: usize = 6;

#[derive(Debug)]
pub struct Digit {
    segments: Vec<u8>,
}

#[derive(Debug)]
pub struct Entry {
    signals: [Digit; 10],
    output: [Digit; 4],
}

pub struct SSDisplay {
    wiring: [u8; 7],
}

impl Digit {
    pub fn is_easy(&self) -> bool {
        let seg_len = self.segments.len();
        seg_len == 2 || seg_len == 3 || seg_len == 4 || seg_len == 7
    }
}

impl Entry {
    pub fn outputs(&self) -> &[Digit] {
        &self.output
    }
    pub fn signals(&self) -> &[Digit] {
        &self.signals
    }

    fn get_signal_one(&self) -> Option<&Digit> {
        self.signals.iter().find(|d| d.segments.len() == 2)
    }

    fn get_signal_four(&self) -> Option<&Digit> {
        self.signals.iter().find(|d| d.segments.len() == 4)
    }

    fn get_signal_seven(&self) -> Option<&Digit> {
        self.signals.iter().find(|d| d.segments.len() == 3)
    }

    fn get_signal_069(&self) -> Vec<&Digit> {
        self.signals
            .iter()
            .filter(|d| d.segments.len() == 6)
            .collect()
    }

    pub fn deduce_wiring(&self) -> Option<[u8; 7]> {
        // see notes.md for more clarifications.

        let one = self.get_signal_one()?;

        let mut zero_six_nine = self.get_signal_069();
        if zero_six_nine.len() != 3 {
            return None;
        }

        let mut wiring = [b'z'; 7]; // init with some "invalid" value

        // Compare 0, 6 and 9 with 1 to determine top-right + bottom-right.
        let mut six_idx = None;
        for (j, digit) in zero_six_nine.iter().enumerate() {
            for (i, val) in one.segments.iter().enumerate() {
                if !digit.segments.contains(val) {
                    // found 6
                    six_idx = Some(j);
                    wiring[TOP_RIGHT] = *val;
                    wiring[BOTTOM_RIGHT] = one.segments[(i + 1).rem_euclid(2)];
                    break;
                }
            }
            if six_idx.is_some() {
                break;
            }
        }
        if let Some(idx) = six_idx {
            zero_six_nine.swap_remove(idx);
        } else {
            return None;
        }

        // Look at 7 for top.
        let seven = self.get_signal_seven()?;
        wiring[TOP] = *seven
            .segments
            .iter()
            .find(|val| !one.segments.contains(val))?;

        // Compare 0 and 9 with 4 to determine the middle.
        let four = self.get_signal_four()?;
        let mut zero_nine_idx = None;
        for (i, digit) in zero_six_nine.iter().enumerate() {
            for val in four.segments.iter() {
                if !digit.segments.contains(val) {
                    // Found 0. Missing piece is middle.
                    wiring[MIDDLE] = *val;
                    zero_nine_idx = Some((i, (i + 1).rem_euclid(2)));
                    break;
                }
            }
            if zero_nine_idx.is_some() {
                break;
            }
        }

        // Look at 4 for top-left.
        wiring[TOP_LEFT] = *four.segments.iter().find(|val| !wiring.contains(val))?;

        // Look at 0 and 9 to find bottom-left.
        if let Some((zidx, nidx)) = zero_nine_idx {
            let zero = zero_six_nine[zidx];
            let nine = zero_six_nine[nidx];
            wiring[BOTTOM_LEFT] = *zero
                .segments
                .iter()
                .find(|val| !nine.segments.contains(val) && **val != wiring[MIDDLE])?;
        } else {
            return None;
        }

        // Last unmatched value (0 -> 6) is bottom
        // FIXME: check for error
        for v in 0..=6 {
            if !wiring.contains(&v) {
                wiring[BOTTOM] = v;
                break;
            }
        }
        Some(wiring)
    }
}

impl SSDisplay {
    pub fn flawed(wiring: [u8; 7]) -> SSDisplay {
        SSDisplay { wiring }
    }

    fn digit_value(&self, d: &Digit) -> Option<u32> {
        match d.segments.len() {
            2 => Some(1),
            3 => Some(7),
            4 => Some(4),
            7 => Some(8),
            5 => {
                // 2, 3 or 5
                if d.segments.contains(&self.wiring[TOP_LEFT]) {
                    Some(5)
                } else if d.segments.contains(&self.wiring[BOTTOM_LEFT]) {
                    Some(2)
                } else {
                    Some(3)
                }
            }
            6 => {
                // 0, 6 or 9
                if !d.segments.contains(&self.wiring[MIDDLE]) {
                    Some(0)
                } else if !d.segments.contains(&self.wiring[TOP_RIGHT]) {
                    Some(6)
                } else {
                    Some(9)
                }
            }
            _ => None,
        }
    }

    pub fn value(&self, digits: &[Digit]) -> Option<u32> {
        let mut value = 0;
        for d in digits {
            value *= 10;
            value += self.digit_value(d)?;
        }
        Some(value)
    }
}

impl FromStr for Digit {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        // validate characters
        for bc in s.bytes() {
            if !(b'a'..=b'g').contains(&bc) {
                return Err(InputError::ParseGeneral);
            }
        }
        let mut segs: Vec<u8> = s.into();
        segs.sort_unstable();
        segs.iter_mut().for_each(|e| *e -= b'a');
        Ok(Digit { segments: segs })
    }
}

impl FromStr for Entry {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if let Some((l, r)) = s.split_once(" | ") {
            let signals = l
                .trim()
                .splitn(10, ' ')
                .map(|s| s.parse::<Digit>())
                .collect::<Result<Vec<Digit>, InputError>>()?;
            let output = r
                .trim()
                .splitn(4, ' ')
                .map(|s| s.parse::<Digit>())
                .collect::<Result<Vec<Digit>, InputError>>()?;
            // FIXME replace unwraps
            Ok(Entry {
                signals: signals.try_into().unwrap(),
                output: output.try_into().unwrap(),
            })
        } else {
            Err(InputError::ParseGeneral)
        }
    }
}
