/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-16
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"

#define BUFFER_LENGTH (size_t)(20)
#define REACTION_DELIM "=>"
#define MATERIALS_DELIM ","

static int count_lines(const char* const file_path);
static int count_inputs(const char* const left_token);
static void parse_line(char* const line, Reaction* const output);
static void parse_token(char* token, Material* const output);
static Material* copy_material(const Material* const mat);
static Material** copy_inputs(const Reaction* const reaction);
static Reaction* get_reaction(const char* const name, const ReactionList* const list);
static Reaction* create_reaction();
static Material* create_material();
static void
reduce_with_stash(Material* const needed, Material* const* const stash, const int stash_size);
static void
add_to_stash(Material* const needed, Material* const* const stash, const int stash_size);

Reaction** parse_input(const char* const file_path, int* const num_reactions)
{
    Reaction** reactions = NULL;
    if ((file_path != NULL) && (num_reactions != NULL))
    {
        int num_lines = count_lines(file_path);

        reactions = (Reaction**)malloc(sizeof(Reaction*) * num_lines);
        if (reactions == NULL)
        {
            return NULL;
        }

        FILE* fp = fopen(file_path, "r");
        if (fp == NULL)
        {
            return NULL;
        }

        char* line       = (char*)malloc(sizeof(char) * BUFFER_LENGTH);
        int num_attempts = 0;
        int line_index   = 0;
        int offset       = 0;
        while ((line != NULL) && (fgets(line + offset, BUFFER_LENGTH, fp) != NULL) &&
               (line_index < num_lines))
        {
            size_t length = strlen(line);
            if (line[length - 1] == '\n')
            {
                num_attempts = 0;
                offset       = 0;

                Reaction* reaction = create_reaction();
                parse_line(line, reaction);
                reactions[line_index++] = reaction;

                free(line);
                line = (char*)malloc(sizeof(char) * BUFFER_LENGTH);
            }
            else
            {
                num_attempts++;
                offset = strlen(line);
                line   = (char*)realloc(line, sizeof(char) * BUFFER_LENGTH * (num_attempts + 1));
            }
        }
        if (line != NULL)
        {
            free(line);
        }
        fclose(fp);
        *num_reactions = line_index;
    }
    return reactions;
}

Material** initialize_stash(const ReactionList* const list)
{
    Material** stash = NULL;
    if ((list != NULL) && (list->reactions != NULL))
    {
        stash = (Material**)malloc(sizeof(Material*) * list->size);
        for (int i = 0; i < list->size; ++i)
        {
            Reaction* r = list->reactions[i];
            if ((r != NULL) && (r->output != NULL))
            {
                Material* mat = copy_material(r->output);
                if (mat != NULL)
                {
                    mat->amount = 0;
                    stash[i]    = mat;
                }
            }
        }
    }
    return stash;
}

void print_reaction_list(const ReactionList* const list)
{
    if (list == NULL)
    {
        return;
    }
    for (int i = 0; i < list->size; ++i)
    {
        print_reaction(list->reactions[i]);
    }
}

static void
reduce_with_stash(Material* const needed, Material* const* const stash, const int stash_size)
{
    assert(needed != NULL);
    assert(stash != NULL);

    for (int i = 0; i < stash_size; ++i)
    {
        Material* stashed = stash[i];

        if (strcmp(stashed->name, needed->name) == 0)
        {
            while ((stashed->amount > 0) && (needed->amount > 0))
            {
                stashed->amount--;
                needed->amount--;
            }
            break;
        }
    }
}

static void add_to_stash(Material* const add, Material* const* const stash, const int stash_size)
{
    assert(add != NULL);
    assert(stash != NULL);
    for (int i = 0; i < stash_size; ++i)
    {
        Material* stashed = stash[i];

        if (strcmp(stashed->name, add->name) == 0)
        {
            if (add->amount > 0)
            {
                stashed->amount += add->amount;
            }
            break;
        }
    }
}

int reduce_to(const Material* const from,
              const char* const to,
              const ReactionList* const list,
              Material* const* const stash)
{
    int total = 0;
    if ((from != NULL) && (to != NULL) && (list != NULL) && (stash != NULL))
    {
#ifdef DEBUG
        printf("Reduce %d %s to %s", from->amount, from->name, to);
#endif

        Material* from_cpy = copy_material(from);
        if (from_cpy == NULL)
        {
            return 0;
        }

        /*Check if the 'from' material can be satisfied with stash*/
        reduce_with_stash(from_cpy, stash, list->size);

        int reduced_amount = from_cpy->amount;
        destroy_material(from_cpy);

        if (reduced_amount <= 0)
        {
#ifdef DEBUG
            printf("\n");
#endif
            return 0;
        }

        /*Get required reaction for 'from'.*/
        Reaction* r = get_reaction(from->name, list);

        if (r != NULL)
        {
            /*Copy inputs and determine required applications.*/
            Material** inputs = copy_inputs(r);
            if (inputs == NULL)
            {
                return 0;
            }
            int r_amount     = r->output->amount;
            int applications = 1;
            int extra        = 0;
            /*printf("Needed amount: %d, produced amount with single application: %d\n", reduced_amount, r_amount);*/
            while (r_amount < reduced_amount)
            {
                r_amount += r->output->amount;
                applications++;
            }
            /*printf("Produced amount with %d applications: %d\n", applications, r_amount);*/
            extra = r_amount - reduced_amount;

            /*Multiply for required amount and Substract stashed materials from inputs.*/
            for (int i = 0; i < r->input_size; ++i)
            {
                inputs[i]->amount *= applications;
                reduce_with_stash(inputs[i], stash, list->size);
            }

            /*If the input is ORE return.*/
            if ((r->input_size == 1) && (strcmp(r->inputs[0]->name, "ORE") == 0))
            {
                total = r->inputs[0]->amount * applications;
#ifdef DEBUG
                printf(" => Direct ORE conversion: %d ORE.\n", total);
#endif
            }
            /*Else iterate over inputs and recursively call reduce and sum up to total*/
            else
            {
#ifdef DEBUG
                printf("\n");
#endif
                for (int i = 0; i < r->input_size; ++i)
                {
                    total += reduce_to(inputs[i], to, list, stash);
                }
            }

            /*Add extra material to stash.*/
            Material add = {.amount = extra, .name = from->name};
            add_to_stash(&add, stash, list->size);


            for (int i = 0; i < r->input_size; ++i)
            {
                destroy_material(inputs[i]);
            }
            free(inputs);
        }
    }
    return total;
}

void destroy_reaction_list(ReactionList* const list)
{
    if (list != NULL)
    {
        if (list->reactions != NULL)
        {
            for (int i = 0; i < list->size; ++i)
            {
                destroy_reaction(list->reactions[i]);
            }
            free(list->reactions);
        }
        free(list);
    }
}

void destroy_reaction(Reaction* const reaction)
{
    if (reaction != NULL)
    {
        destroy_material(reaction->output);
        if (reaction->inputs != NULL)
        {
            for (int i = 0; i < reaction->input_size; ++i)
            {
                destroy_material(reaction->inputs[i]);
            }
            free(reaction->inputs);
        }
        free(reaction);
    }
}

void destroy_material(Material* const material)
{
    if (material != NULL)
    {
        if (material->name != NULL)
        {
            free(material->name);
        }
        free(material);
    }
}

static Reaction* get_reaction(const char* const name, const ReactionList* const list)
{
    Reaction* r = NULL;
    if ((name != NULL) && (list != NULL) && (list->reactions != NULL))
    {
        for (int i = 0; i < list->size; ++i)
        {
            if (strcmp(name, list->reactions[i]->output->name) == 0)
            {
                r = list->reactions[i];
                break;
            }
        }
    }
    return r;
}

static void parse_line(char* const line, Reaction* const output)
{
    assert(line != NULL);
    assert(output != NULL);

    char* left_token  = strtok(line, REACTION_DELIM);
    char* right_token = strtok(NULL, REACTION_DELIM);

    if ((left_token != NULL) && (right_token != NULL))
    {
        /*parse left side of reaction*/
        int num_of_inputs = count_inputs(left_token);
        Material** inputs = (Material**)malloc(sizeof(Material*) * num_of_inputs);
        if (inputs == NULL)
        {
            return;
        }

        char* material_token = strtok(left_token, MATERIALS_DELIM);
        while (material_token != NULL)
        {
            Material* mat = create_material();
            parse_token(material_token, mat);
            inputs[output->input_size++] = mat;
            material_token               = strtok(NULL, MATERIALS_DELIM);
        }
        output->inputs = inputs;

        /*parse right side of reaction*/
        Material* mat = create_material();
        parse_token(right_token, mat);
        output->output = mat;
    }
}

static void parse_token(char* token, Material* const output)
{
    assert(token != NULL);
    assert(output != NULL);

    /*Strip token of leading and trailing whitespaces and linebreaks*/
    int len     = strlen(token);
    int trimmed = 0;
    for (int i = 0; i < len; ++i)
    {
        if (token[i] == ' ')
        {
            trimmed++;
        }
        else
        {
            break;
        }
    }
    token += trimmed;
    len     = strlen(token);
    trimmed = len;
    for (int i = len - 1; i > 0; i--)
    {
        if ((token[i] == ' ') || (token[i] == '\n'))
        {
            trimmed--;
        }
        else
        {
            break;
        }
    }
    token[trimmed] = '\0';

    char* id = (char*)malloc(strlen(token) - 2 + 1);
    if (id != NULL)
    {
        int amount = 0;
        sscanf(token, "%d %s", &amount, id);

        output->name   = id;
        output->amount = amount;
    }
}

static Material* create_material()
{
    Material* mat = (Material*)malloc(sizeof(Material));
    if (mat != NULL)
    {
        mat->amount = 0;
        mat->name   = NULL;
    }
    return mat;
}

static Reaction* create_reaction()
{
    Reaction* reaction = (Reaction*)malloc(sizeof(Reaction));
    if (reaction != NULL)
    {
        reaction->input_size = 0;
        reaction->inputs     = NULL;
        reaction->output     = NULL;
    }
    return reaction;
}

static int count_lines(const char* const file_path)
{
    assert(file_path != NULL);
    /*Ugly and inefficient to read the same file twice, but I'm*/
    /*currently too lazy to dynamically adjust the memory needed.
     *And I don't want to hardcode the amount either.*/
    int num_lines = 0;
    FILE* fp;
    fp = fopen(file_path, "r");
    if (fp != NULL)
    {
        char c = fgetc(fp);
        while (!feof(fp))
        {
            if (c == '\n')
            {
                num_lines++;
            }
            c = fgetc(fp);
        }
        fclose(fp);
    }
    return num_lines;
}

void print_reaction(const Reaction* const reaction)
{
    if ((reaction != NULL) && (reaction->inputs != NULL) && (reaction->output != NULL))
    {
        for (int i = 0; i < reaction->input_size; ++i)
        {
            printf("%d %s ", reaction->inputs[i]->amount, reaction->inputs[i]->name);
        }
        printf("=> %d %s\n", reaction->output->amount, reaction->output->name);
    }
}

static int count_inputs(const char* const left_token)
{
    assert(left_token != NULL);

    int count = 1;
    for (int i = 0; i < strlen(left_token); ++i)
    {
        if (left_token[i] == ',')
        {
            count++;
        }
    }
    return count;
}

static Material* copy_material(const Material* const mat)
{
    assert(mat != NULL);
    Material* copy = create_material();
    if (copy != NULL)
    {
        copy->amount = mat->amount;
        copy->name   = (char*)malloc(sizeof(char) * strlen(mat->name) + 1);
        if (copy->name != NULL)
        {
            strcpy(copy->name, mat->name);
        }
    }
    return copy;
}

static Material** copy_inputs(const Reaction* const reaction)
{
    assert(reaction != NULL);
    assert(reaction->inputs != NULL);
    Material** inputs = (Material**)malloc(sizeof(Material*) * reaction->input_size);
    if (inputs != NULL)
    {
        for (int i = 0; i < reaction->input_size; ++i)
        {
            inputs[i] = copy_material(reaction->inputs[i]);
        }
    }
    return inputs;
}
