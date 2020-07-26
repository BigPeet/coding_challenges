/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-16
 *
 */

#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_14 FILE_PATH.\n");
        return 0;
    }

    int num_of_reactions = 0;
    ReactionList* list   = (ReactionList*) malloc(sizeof(ReactionList));
    if (list == NULL)
    {
        return 0;
    }

    list->reactions = parse_input(argv[1], &num_of_reactions);
    if (list->reactions == NULL)
    {
        return 0;
    }
    list->size = num_of_reactions;

    Material** stash = initialize_stash(list);

    /*Part 1*/
    char* to         = "ORE";
    Material fuel    = {.amount = 1, .name = "FUEL"};
    int ore_per_fuel = reduce_to(&fuel, to, list, stash);
    printf("Total %d of %s required to produce %ld %s\n", ore_per_fuel, to, fuel.amount, fuel.name);

    /*Clean up stash*/
    for (int i = 0; i < list->size; ++i)
    {
        destroy_material(stash[i]);
    }
    free(stash);

    /*Part 2*/
    /*Create new stash.*/
    int64_t ore_storage = 1000000000000;
    int64_t total_fuel  = produce(fuel.name, ore_storage, ore_per_fuel, list);
    printf("%ld ORE produce %ld amount of FUEL.\n", ore_storage, total_fuel);


    /*Clean up*/
    destroy_reaction_list(list);
    return 0;
}
