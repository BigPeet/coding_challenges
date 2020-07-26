/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-16
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stdint.h"

typedef struct
{
    int64_t amount;
    char* name;
} Material;

typedef struct
{
    Material** inputs;
    Material* output;
    int input_size;
} Reaction;

typedef struct
{
    Reaction** reactions;
    int size;
} ReactionList;


Reaction** parse_input(const char* const file_path, int* const num_reactions);
Material** initialize_stash(const ReactionList* const list);

void destroy_reaction_list(ReactionList* const list);
void destroy_reaction(Reaction* const reaction);
void destroy_material(Material* const material);
void print_reaction(const Reaction* const reaction);
void print_reaction_list(const ReactionList* const list);
void print_material(const Material* const mat);
void print_stash(Material* const* const stash, const int stash_size);

int64_t reduce_to(const Material* const from,
                  const char* const to,
                  const ReactionList* const list,
                  Material* const* const stash);

int64_t produce(const char* const target,
                const int64_t ore_storage,
                const int64_t one_time_ore_per_fuel,
                const ReactionList* const list);


#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
