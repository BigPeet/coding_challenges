use parsing::InputError;
use std::collections::{HashMap, HashSet, VecDeque};
use std::str::FromStr;

#[derive(Debug)]
pub struct Food {
    ingredients: Vec<String>,
    allergenes: Vec<String>,
}

impl FromStr for Food {
    type Err = InputError;

    fn from_str(s: &str) -> Result<Self, Self::Err> {
        let mut allergenes = vec![];
        let mut ingredients = vec![];
        let ingredients_s;
        if let Some((l, r)) = s.split_once("(contains ") {
            ingredients_s = l.trim_end();
            for tok in r.trim_end().trim_end_matches(')').split(',') {
                allergenes.push(tok.trim().to_string());
            }
        } else {
            ingredients_s = s.trim_end();
        }

        for tok in ingredients_s.split_whitespace() {
            ingredients.push(tok.to_string());
        }

        Ok(Food {
            ingredients,
            allergenes,
        })
    }
}

impl Food {
    pub fn ingredients(&self) -> &[String] {
        &self.ingredients
    }
}

pub struct FoodList(pub Vec<Food>);

impl FoodList {
    pub fn ingredient_set(&self) -> HashSet<&String> {
        let mut ingredients = HashSet::new();
        for f in self.0.iter() {
            f.ingredients.iter().for_each(|ing| {
                ingredients.insert(ing);
            });
        }
        ingredients
    }

    fn clean_up(
        allergene: &str,
        amap: &mut HashMap<&str, Vec<String>>,
        found: &mut Vec<TranslatedAllergene>,
    ) {
        // If there is only one candidate for this allergene, remove the candidate from all
        // other map entries.
        let mut queue = VecDeque::new();
        if let Some(candidates) = amap.get(allergene) {
            // FIXME: redundant
            if candidates.len() == 1 {
                let translated = TranslatedAllergene {
                    name: allergene.to_string(),
                    foreign_name: candidates[0].clone(),
                };
                queue.push_back(translated);
            }
        }
        while let Some(popped) = queue.pop_back() {
            for (k, v) in amap.iter_mut() {
                if **k != popped.name && v.len() > 1 {
                    v.retain(|ing| *ing != popped.foreign_name);
                    if v.len() == 1 {
                        let translated = TranslatedAllergene {
                            name: (**k).to_string(),
                            foreign_name: v[0].clone(),
                        };
                        queue.push_back(translated);
                    }
                }
            }
            found.push(popped);
        }
    }

    pub fn translate(&self) -> Vec<TranslatedAllergene> {
        let mut translated_allergenes = vec![];
        let mut allergene_map: HashMap<&str, Vec<String>> = HashMap::new();
        for food in self.0.iter().filter(|f| !f.allergenes.is_empty()) {
            for a in food.allergenes.iter() {
                // remove any ingredient not present in this food or add all ingredients as candidates
                if let Some(candidates) = allergene_map.get_mut(a.as_str()) {
                    if candidates.len() > 1 {
                        candidates.retain(|ing| food.ingredients.contains(ing));
                        if candidates.len() == 1 {
                            Self::clean_up(a, &mut allergene_map, &mut translated_allergenes);
                        }
                    }
                } else {
                    allergene_map.insert(a, food.ingredients.clone());
                }
            }
        }
        translated_allergenes
    }
}

#[derive(Debug)]
pub struct TranslatedAllergene {
    name: String,
    foreign_name: String,
}

impl TranslatedAllergene {
    pub fn name(&self) -> &str {
        &self.name
    }

    pub fn foreign_name(&self) -> &str {
        &self.foreign_name
    }
}
