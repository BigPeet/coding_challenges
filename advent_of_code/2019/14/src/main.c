/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-16
 *
 */

#include "challenge/challenge_lib.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"

int main(int argc, char* argv[])
{
    if (argc != 2)
    {
        printf("This executabel takes exactly one argument.\n");
        printf("Usage: aoc2019_14 FILE_PATH.\n");
        return 0;
    }

    int num_of_reactions = 0;
    ReactionList* list   = (ReactionList*)malloc(sizeof(ReactionList));
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

    char* to      = "ORE";
    Material fuel = {.amount = 1, .name = "FUEL"};
    int total     = reduce_to(&fuel, to, list, stash);
    printf("Total %d of %s required to produce %d %s\n", total, to, fuel.amount, fuel.name);

    /*Clean up*/
    for (int i = 0; i < list->size; ++i)
    {
        destroy_material(stash[i]);
    }
    free(stash);
    destroy_reaction_list(list);
    return 0;
}
