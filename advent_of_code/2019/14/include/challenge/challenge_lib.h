/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-16
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

typedef struct
{
    int amount;
    char* name;
} Material;

typedef struct
{
    Material* inputs;
    Material output;
} Reaction;


Reaction* parse_input(const char* const file_path, int* const num_reactions);

Material* initialize_stash(const Reaction* const reactions);
void destroy_reaction(Reaction* const reaction);
void destroy_material(Material* const material);



#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
