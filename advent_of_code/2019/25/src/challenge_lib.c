/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *
 */

#include "assert.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "challenge/challenge_lib.h"
#include "challenge/intcode.h"

#define MAX_COMMAND_LENGTH (100)
#define MAX_OUTPUT_LENGTH (1000)
#define NUM_OF_ITEMS (8)
#define INV_CMD ("inv\n")

static int64_t read_output(const ASCII* const system)
{
    assert(system != NULL);
    assert(system->brain != NULL);
    assert(system->brain->mem_io_out != NULL);

    int64_t read_value = -1;

    intcode_io_mem_t* output = system->brain->mem_io_out;
    pthread_mutex_lock(&output->mut);
    while (output->consumed && !system->finished)
    {
        pthread_cond_wait(&output->cond, &output->mut);
    }

    if (!system->finished)
    {
        read_value       = output->value;
        output->consumed = 1;
    }
    pthread_cond_signal(&output->cond);
    pthread_mutex_unlock(&output->mut);


    return read_value;
}

static void provide_input(const ASCII* const system, const int value)
{
    assert(system != NULL);
    assert(system->brain != NULL);
    assert(system->brain->mem_io_in != NULL);

    intcode_io_mem_t* input = system->brain->mem_io_in;
    pthread_mutex_lock(&input->mut);
    while (!input->consumed && !system->finished)
    {
        pthread_cond_wait(&input->cond, &input->mut);
    }
    input->value    = value;
    input->consumed = 0;
    pthread_cond_signal(&input->cond);
    pthread_mutex_unlock(&input->mut);
}

/*Read command prompt. Afterwards either finished or expecting input.*/
static void read_prompt(const ASCII* const system, int print_output)
{
    while (!waiting_for_input(system->brain) && !system->finished)
    {
        if (providing_ouput(system->brain))
        {
            /*Read Response*/
            char resp = (char) read_output(system);
            if (print_output)
            {
                printf("%c", resp);
            }
        }
    }
}

/*Read command prompt. Afterwards either finished or expecting input.*/
static void read_prompt_to_buffer(const ASCII* const system, char* buffer, int* idx)
{
    while (!waiting_for_input(system->brain) && !system->finished)
    {
        if (providing_ouput(system->brain))
        {
            /*Read Response*/
            char resp    = (char) read_output(system);
            buffer[*idx] = resp;
            *idx += 1;
        }
    }
}

/*Provide input. Afterwards either finished or providing output.*/
static void provide_line(const ASCII* const system, const char* const line)
{
    assert(system != NULL);
    assert(system->brain != NULL);
    assert(line != NULL);
    for (int i = 0; i < strlen(line); ++i)
    {
        provide_input(system, (int64_t) line[i]);
    }
    while (!system->finished && !providing_ouput(system->brain))
    {
        /*spin to ensure the provided line has been evaluated*/
    }
}

static void parse_inv(char* inv, char** items)
{
    // inv is expected to look something like this with at most 8 items:
    // Items in your inventory:
    // - A
    // - B
    // - C
    //
    // Command?

    // split inv string into lines
    char* token = strtok(inv, "\n");
    int counter = 0;
    while (token != NULL)
    {
        // Starts with - => take substring from 3rd to end
        if (token[0] == '-')
        {
            size_t len     = strlen(token + 2);
            items[counter] = (char*) malloc(sizeof(char) * (len + 1));
            strcpy(items[counter], token + 2);
            counter++;
        }
        token = strtok(NULL, "\n");
    }
}

static void take(const ASCII* const system, char const* item)
{
    char cmd[MAX_COMMAND_LENGTH];
    cmd[0] = '\0';
    strcat(cmd, "take ");
    strcat(cmd, item);
    strcat(cmd, "\n");
    provide_line(system, cmd);
    read_prompt(system, 0);
}

static void drop(const ASCII* const system, char const* item)
{
    char cmd[MAX_COMMAND_LENGTH];
    cmd[0] = '\0';
    strcat(cmd, "drop ");
    strcat(cmd, item);
    strcat(cmd, "\n");
    provide_line(system, cmd);
    read_prompt(system, 0);
}

static void bruteforce(const ASCII* const system, char** items, char const* cmd)
{
    char resp[MAX_OUTPUT_LENGTH];
    int resp_idx     = 0;
    uint8_t solution = 0u;
    for (uint8_t i = 0u; i < 255u; ++i)
    {
        /*prepare inventory*/
        for (uint8_t j = 0u; j < NUM_OF_ITEMS; ++j)
        {
            ((i >> j) & 1u) ? take(system, items[j]) : drop(system, items[j]);
        }
        /*issue command to move into room*/
        provide_line(system, cmd);

        /*read answer*/
        read_prompt_to_buffer(system, resp, &resp_idx);
        if (system->finished)
        {
            resp[resp_idx] = '\0';
            solution       = i;
            break;
        }
        resp_idx = 0;
    }
    printf("\nFound solution using the following items:\n");
    for (uint8_t i = 0; i < NUM_OF_ITEMS; ++i)
    {
        if ((solution >> i) & 1u)
        {
            printf("- %s\n", items[i]);
        }
    }
    printf("\nFinal Message:\n%s", resp);
}

static void prompt_and_command_loop(const ASCII* const system,
                                    FILE* input_stream,
                                    int verbose_output,
                                    char* last_command)
{
    /*Prompt and Command loop*/
    char cmd_buffer[MAX_COMMAND_LENGTH];
    int input_definitely_consumed = 0;
    while (!system->finished && !feof(input_stream))
    {
        if (providing_ouput(system->brain))
        {
            input_definitely_consumed = 1;
            read_prompt(system, verbose_output);
        }
        else if (waiting_for_input(system->brain) && input_definitely_consumed)
        {
            if (fgets(cmd_buffer, MAX_COMMAND_LENGTH, input_stream))
            {
                if (input_stream != stdin && verbose_output)
                {
                    printf("%s", cmd_buffer);
                }
                provide_line(system, cmd_buffer);
                input_definitely_consumed = 0;
            }
        }
    }
    if (last_command)
    {
        strcpy(last_command, cmd_buffer);
    }
}

static void run_interactive_program(const ASCII* const system)
{
    prompt_and_command_loop(system, stdin, 1, NULL);
}

static void run_hardcoded_commands(const ASCII* const system,
                                   char const* command_file,
                                   int bruteforce_required,
                                   int verbose_output)
{
    FILE* fp = fopen(command_file, "r");
    if (fp)
    {
        char last_command[MAX_COMMAND_LENGTH];
        last_command[0] = '\0';
        prompt_and_command_loop(system, fp, (verbose_output || !bruteforce_required), last_command);
        fclose(fp);

        printf("Finished command list.\n");

        while (!system->finished && bruteforce_required && !waiting_for_input(system->brain))
        {
            /*spin until input is required*/
        }

        if (!system->finished && bruteforce_required && waiting_for_input(system->brain))
        {
            printf("Bruteforcing!\n");

            /*Get list of items, should be all 8.*/
            char output_buffer[MAX_OUTPUT_LENGTH];
            int output_idx = 0;
            provide_line(system, INV_CMD);
            while (!system->finished && output_idx == 0)
            {
                if (providing_ouput(system->brain))
                {
                    read_prompt_to_buffer(system, output_buffer, &output_idx);
                }
            }
            output_buffer[output_idx] = '\0';

            // parse output of inv command
            char* items[NUM_OF_ITEMS];
            parse_inv(output_buffer, items);
            printf("\nCollected Items:\n");
            for (int i = 0; i < NUM_OF_ITEMS; ++i)
            {
                printf("- %s\n", items[i]);
            }

            bruteforce(system, items, last_command);

            for (int i = 0; i < NUM_OF_ITEMS; ++i)
            {
                if (items[i])
                {
                    free(items[i]);
                }
            }
        }
    }
}


void* drone_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    ASCII* system = (ASCII*) args;
    int ret       = execute(system->brain);
    if (ret != INT_CODE_HALT)
    {
        printf("Programm did not halt as expected. Err code: %d\n", ret);
    }
    else
    {
        system->finished = 1;
    }
    return NULL;
}

void* control_func(void* args)
{
    if (args == NULL)
    {
        return NULL;
    }
    ControlParams* control_params = (ControlParams*) args;
    if (control_params->interactive)
    {
        run_interactive_program(control_params->drone);
    }
    else
    {
        run_hardcoded_commands(control_params->drone,
                               control_params->command_file,
                               control_params->bruteforce,
                               control_params->verbose);
    }
    return NULL;
}
