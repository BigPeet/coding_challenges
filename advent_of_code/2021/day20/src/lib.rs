use parsing::InputError;
use std::convert::TryFrom;
use std::fmt::Display;
use std::str::FromStr;

#[derive(Debug, Copy, Clone)]
enum Pixel {
    Light,
    Dark,
}

impl TryFrom<char> for Pixel {
    type Error = InputError;

    fn try_from(value: char) -> Result<Self, Self::Error> {
        match value {
            '#' => Ok(Pixel::Light),
            '.' => Ok(Pixel::Dark),
            _ => Err(InputError::ParseGeneral),
        }
    }
}

impl Display for Pixel {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            Pixel::Light => write!(f, "#"),
            Pixel::Dark => write!(f, "."),
        }
    }
}

#[derive(Debug)]
pub struct Algorithm([Pixel; 512]);

impl FromStr for Algorithm {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        if s.len() < 512 {
            return Err(InputError::ParseGeneral);
        }
        let mut algo = [Pixel::Dark; 512];
        for (c, v) in s.chars().zip(algo.iter_mut()) {
            *v = Pixel::try_from(c)?;
        }
        Ok(Algorithm(algo))
    }
}

impl Algorithm {
    fn calculate(&self, bin_num: &[Pixel; 9]) -> Pixel {
        let s = bin_num
            .iter()
            .map(|p| match p {
                Pixel::Light => '1',
                Pixel::Dark => '0',
            })
            .collect::<String>();
        let idx = usize::from_str_radix(&s, 2).expect("Can't convert to binary!");
        self.0[idx]
    }
}

#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash)]
struct Position(i32, i32);

pub struct Image {
    pixels: Vec<Pixel>,
    width: usize,
    height: usize,
}

impl FromStr for Image {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut pixels = Vec::new();
        let mut cur_pos = Position(0, 0);
        for c in s.trim().chars() {
            if c == '\n' {
                cur_pos.0 = 0;
                cur_pos.1 += 1;
            } else {
                pixels.push(Pixel::try_from(c)?);
                cur_pos.0 += 1;
            }
        }
        Ok(Image {
            pixels,
            width: cur_pos.0 as usize,
            height: (cur_pos.1 + 1) as usize,
        })
    }
}

impl Display for Image {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        for y in 0..self.height {
            for x in 0..self.width {
                write!(f, "{}", self.pixels[x + y * self.width])?;
            }
            if y < self.width - 1 {
                writeln!(f)?;
            }
        }
        Ok(())
    }
}

impl Image {
    fn get_pixel(&self, pos: &Position) -> Option<Pixel> {
        if pos.0 >= 0
            && (pos.0 as usize) < self.width
            && pos.1 >= 0
            && (pos.1 as usize) < self.height
        {
            let idx = (pos.0 as usize) + (pos.1 as usize) * self.width;
            Some(self.pixels[idx])
        } else {
            None
        }
    }

    pub fn enhance_twice(&self, algo: &Algorithm) -> Option<Image> {
        if !matches!(algo.0[0], Pixel::Light) {
            Some(
                self.enhance_with(algo, Pixel::Dark)
                    .enhance_with(algo, Pixel::Dark),
            )
        } else if matches!(algo.0[511], Pixel::Dark) {
            // Oscillating
            Some(
                self.enhance_with(algo, Pixel::Dark)
                    .enhance_with(algo, Pixel::Light),
            )
        } else {
            // Don't handle this case!
            None
        }
    }

    fn enhance_with(&self, algo: &Algorithm, border_pixel: Pixel) -> Image {
        // For each pixel in the output, calculate value from input+algo.
        let mut output_pixels = Vec::with_capacity((self.height + 2) * (self.width + 2));
        for y in 0..self.height + 2 {
            for x in 0..self.width + 2 {
                let pos = Position(x as i32, y as i32);
                let mut bin_num = [Pixel::Dark; 9];
                for (i, input) in InputPixels::input_pixels(pos).enumerate() {
                    let offset_pos = Position(input.0 - 1, input.1 - 1);
                    bin_num[i] = self.get_pixel(&offset_pos).unwrap_or(border_pixel);
                }
                output_pixels.push(algo.calculate(&bin_num));
            }
        }

        Image {
            pixels: output_pixels,
            width: self.width + 2,
            height: self.height + 2,
        }
    }

    pub fn lit_pixels(&self) -> usize {
        self.pixels
            .iter()
            .filter(|p| matches!(p, Pixel::Light))
            .count()
    }
}

struct InputPixels {
    center: Position,
    index: usize,
}

impl InputPixels {
    const LENGTH: usize = 9;
    const OFFSETS: [Position; Self::LENGTH] = [
        Position(-1, -1),
        Position(0, -1),
        Position(1, -1),
        Position(-1, 0),
        Position(0, 0),
        Position(1, 0),
        Position(-1, 1),
        Position(0, 1),
        Position(1, 1),
    ];

    fn input_pixels(center: Position) -> impl Iterator<Item = Position> {
        InputPixels { center, index: 0 }
    }
}

impl Iterator for InputPixels {
    type Item = Position;

    fn next(&mut self) -> Option<Self::Item> {
        if self.index < Self::LENGTH {
            let offset = Self::OFFSETS[self.index];
            self.index += 1;
            Some(Position(self.center.0 + offset.0, self.center.1 + offset.1))
        } else {
            None
        }
    }
}
