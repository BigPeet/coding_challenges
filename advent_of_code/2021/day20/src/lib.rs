use parsing::InputError;
use std::collections::HashMap;
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
    pixels: HashMap<Position, Pixel>,
}

impl FromStr for Image {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut pixels = HashMap::new();
        let mut cur_pos = Position(0, 0);
        for c in s.trim().chars() {
            if c == '\n' {
                cur_pos.0 = 0;
                cur_pos.1 += 1;
            } else {
                pixels.insert(cur_pos, Pixel::try_from(c)?);
                cur_pos.0 += 1;
            }
        }
        Ok(Image { pixels })
    }
}

impl Display for Image {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        let mut min_x = 0;
        let mut min_y = 0;
        let mut max_x = 0;
        let mut max_y = 0;

        for pos in self.pixels.keys() {
            if pos.0 < min_x {
                min_x = pos.0;
            }
            if pos.0 > max_x {
                max_x = pos.0;
            }
            if pos.1 < min_y {
                min_y = pos.1;
            }
            if pos.1 > max_y {
                max_y = pos.1;
            }
        }

        for y in min_y..=max_y {
            for x in min_x..=max_x {
                let pixel = self.pixels.get(&Position(x, y)).unwrap_or(&Pixel::Dark);
                write!(f, "{}", pixel)?;
            }
            if y < max_y {
                write!(f, "\n")?;
            }
        }
        Ok(())
    }
}

impl Image {
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
            None
        }
    }

    fn enhance_with(&self, algo: &Algorithm, border_pixel: Pixel) -> Image {
        let mut output_pixels = HashMap::new();

        // extend region to next layer
        for pos in self.pixels.keys() {
            output_pixels.insert(*pos, Pixel::Dark);
            for neighbour in InputPixels::input_pixels(*pos) {
                output_pixels.insert(neighbour, Pixel::Dark);
            }
        }

        // For each pixel in the output, calculate value from input+algo.
        for (pos, pixel) in output_pixels.iter_mut() {
            let mut bin_num = [Pixel::Dark; 9];
            for (i, input) in InputPixels::input_pixels(*pos).enumerate() {
                bin_num[i] = *self.pixels.get(&input).unwrap_or(&border_pixel);
            }
            *pixel = algo.calculate(&bin_num);
        }

        Image {
            pixels: output_pixels,
        }
    }

    pub fn lit_pixels(&self) -> usize {
        self.pixels
            .iter()
            .filter(|(_, p)| matches!(p, Pixel::Light))
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
