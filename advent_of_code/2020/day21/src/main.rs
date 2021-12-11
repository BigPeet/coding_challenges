use day21::{Food, FoodList};
use parsing::ParsingResult;

fn main() -> ParsingResult {
    let foods: Vec<Food> = parsing::filepath_from_args(std::env::args().collect())
        .and_then(parsing::get_lines)
        .and_then(parsing::list_of_values)?;

    // Part 1
    let foods = FoodList(foods);
    let mut allergenes = foods.translate();
    let harmless_ingredients: Vec<&String> = foods
        .ingredient_set()
        .into_iter()
        .filter(|ing| !allergenes.iter().any(|a| a.foreign_name() == *ing))
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

    // Part 2
    allergenes.sort_unstable_by(|a, b| a.name().cmp(b.name()));
    let foreign_names: Vec<&str> = allergenes.iter().map(|a| a.foreign_name()).collect();
    println!(
        "Part 2: The sorted canonical dangerous ingredients list is '{}'.",
        foreign_names.join(",")
    );

    Ok(())
}
