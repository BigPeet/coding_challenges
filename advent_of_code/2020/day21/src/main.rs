use day21::{Food, FoodList};
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let foods: Vec<Food> = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    let foods = FoodList(foods);
    let allergenes = foods.translate();
    let harmless_ingredients: Vec<&String> = foods
        .ingredient_set()
        .into_iter()
        .filter(|ing| !allergenes.iter().any(|a| a.translation() == *ing))
        .collect();

    let mut total_times = 0;
    for f in foods.0.iter() {
        for ing in harmless_ingredients.iter() {
            if f.ingredients().iter().any(|fing| fing == *ing) {
                total_times += 1;
            }
        }
    }
    println!(
        "Part 1: The times safe ingredients appear is {}.",
        total_times
    );

    Ok(())
}
