/*
 *
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-17
 *
 */

#include "challenge/challenge_lib.h"
#include "assert.h"
#include "stdio.h"
#include "stdlib.h"

typedef enum
{
    SKIP,
    ADD,
    SUB,
} Mode;


static int count_chars(const char* const file_path);
static int add(const Sequence* const seq, const int start, const int end);
static int sub(const Sequence* const seq, const int start, const int end);
static void calculate_result(const Sequence* const input, const int row, Sequence* const output);
static void run_phase(const Sequence* const input, Sequence* const output, int skip_first_half);


Sequence* read_sequence(const char* const file_path)
{
    Sequence* seq = NULL;
    if (file_path != NULL)
    {
        int size = count_chars(file_path);
        seq      = (Sequence*) malloc(sizeof(Sequence));
        if (seq == NULL)
        {
            return NULL;
        }
        int* numbers = (int*) malloc(sizeof(int) * size);
        if (numbers == NULL)
        {
            free(seq);
            return NULL;
        }
        seq->size    = size;
        seq->numbers = numbers;

        FILE* fp = fopen(file_path, "r");
        if (fp != NULL)
        {
            int index = 0;
            while ((index < size) && (!feof(fp)))
            {
                char c = fgetc(fp);
                int x;
                sscanf(&c, "%d", &x);
                numbers[index++] = x;
            }
            fclose(fp);
        }
    }
    return seq;
}

Sequence* create_sequence(const int size)
{
    Sequence* seq = (Sequence*) malloc(sizeof(Sequence));
    if (seq != NULL)
    {
        int* numbers = (int*) malloc(sizeof(int) * size);
        if (numbers != NULL)
        {
            seq->numbers = numbers;
            seq->size    = size;
        }
        else
        {
            seq->size = 0;
        }
    }
    return seq;
}

void destroy_sequence(Sequence* const seq)
{
    if (seq != NULL)
    {
        if (seq->numbers != NULL)
        {
            free(seq->numbers);
        }
        free(seq);
    }
}

Sequence* copy_sequence(const Sequence* const seq)
{
    Sequence* copy = NULL;
    if ((seq != NULL) && (seq->numbers != NULL))
    {
        copy = (Sequence*) malloc(sizeof(Sequence));
        if (copy != NULL)
        {
            if (seq->size > 0)
            {
                int* copy_numbers = (int*) malloc(sizeof(int) * seq->size);
                if (copy_numbers != NULL)
                {
                    for (int i = 0; i < seq->size; ++i)
                    {
                        copy_numbers[i] = seq->numbers[i];
                    }
                    copy->numbers = copy_numbers;
                    copy->size    = seq->size;
                }
                else
                {
                    copy->size = 0;
                }
            }
            else
            {
                copy->size = 0;
            }
        }
    }
    return copy;
}

void print_sequence(const Sequence* const seq, const int len)
{
    if ((seq != NULL) && (seq->numbers != NULL))
    {
        for (int i = 0; i < seq->size && i < len; ++i)
        {
            printf("%d", seq->numbers[i]);
        }
        printf("\n");
    }
}

Sequence* get_subsequence(const Sequence* const seq, const int start, const int length)
{
    Sequence* sub_seq = NULL;
    if ((seq != NULL) && (seq->numbers != NULL))
    {
        sub_seq = (Sequence*) malloc(sizeof(Sequence));
        if (sub_seq != NULL)
        {
            if (start >= 0 && length > 0 && ((start + length) <= seq->size))
            {
                int* sub_seq_numbers = (int*) malloc(sizeof(int) * length);
                if (sub_seq_numbers != NULL)
                {
                    int sub_idx = 0;
                    for (int i = start; i < (start + length); ++i)
                    {
                        sub_seq_numbers[sub_idx++] = seq->numbers[i];
                    }
                    sub_seq->numbers = sub_seq_numbers;
                    sub_seq->size    = length;
                }
                else
                {
                    sub_seq->size = 0;
                }
            }
            else
            {
                sub_seq->size = 0;
            }
        }
    }
    return sub_seq;
}

int get_value(const Sequence* const seq)
{
    int value = -1;
    if ((seq != NULL) && (seq->numbers != NULL) && (seq->size < 10))
    {
        value    = 0;
        int mult = 1;
        for (int i = seq->size - 1; i >= 0; i--)
        {
            value += seq->numbers[i] * mult;
            mult *= 10;
        }
    }
    return value;
}
Sequence* repeat_sequence(const Sequence* const seq, const int amount)
{
    Sequence* rep_seq = NULL;
    if ((seq != NULL) && (seq->numbers != NULL))
    {
        rep_seq = (Sequence*) malloc(sizeof(Sequence));
        if (rep_seq != NULL)
        {
            if (amount > 0)
            {
                int length           = amount * seq->size;
                int* rep_seq_numbers = (int*) malloc(sizeof(int) * length);
                if (rep_seq_numbers != NULL)
                {
                    for (int i = 0; i < length; ++i)
                    {
                        rep_seq_numbers[i] = seq->numbers[i % seq->size];
                    }
                    rep_seq->numbers = rep_seq_numbers;
                    rep_seq->size    = length;
                }
                else
                {
                    rep_seq->size = 0;
                }
            }
            else
            {
                rep_seq->size = 0;
            }
        }
    }
    return rep_seq;
}

Sequence* run_phases(const Sequence* const input, const int amount, int skip_first_half)
{
    Sequence* output = NULL;
    if (input != NULL && (input->numbers != NULL) && (input->size > 0))
    {
        output              = create_sequence(input->size);
        Sequence* new_input = copy_sequence(input);
        for (int i = 0; i < amount; ++i)
        {
            run_phase(new_input, output, skip_first_half);
            if (i < (amount - 1))
            {
                destroy_sequence(new_input);
                new_input = copy_sequence(output);
            }
        }
        destroy_sequence(new_input);
    }
    return output;
}

static void run_phase(const Sequence* const input, Sequence* const output, int skip_first_half)
{
    if ((input != NULL) && (input->numbers != NULL) && (output != NULL) &&
        (output->numbers != NULL))
    {
        /*I will just assume that the input size is always a multiple of 4 (or the num of modes).*/
        int half_size = input->size / 2;
        int tmp       = 0;
        for (int i = input->size - 1; i >= half_size; i--)
        {
            tmp += input->numbers[i];
            output->numbers[i] = abs(tmp) % 10;
        }

        if (!skip_first_half)
        {
            for (int i = 0; i < half_size; ++i)
            {
                calculate_result(input, i, output);
            }
        }
    }
}

static void calculate_result(const Sequence* const input, const int row, Sequence* const output)
{
    if ((input != NULL) && (input->numbers != NULL) && (output != NULL) &&
        (output->numbers != NULL))
    {
        int k         = 1;
        int processed = 0;
        int mode_idx  = 0;
        int result    = 0;
        Mode modes[4] = {SKIP, ADD, SKIP, SUB};
        while (processed < input->size)
        {
            int n  = row + 1 - k;
            Mode m = modes[mode_idx];

            if (n > 0)
            {
                /*Determine how many numbers to process.*/
                if (n > (input->size - processed))
                {
                    n = input->size - processed;
                }

                if (m == ADD)
                {
                    result += add(input, processed, processed + n);
                }
                else if (m == SUB)
                {
                    result += sub(input, processed, processed + n);
                }
            }

            /*Increase index and handle other variables.*/
            k        = 0;
            mode_idx = (mode_idx + 1) % 4;
            processed += n;
        }
        output->numbers[row] = abs(result) % 10;
    }
}

static int add(const Sequence* const seq, const int start, const int end)
{
    assert(seq != NULL);
    assert(seq->numbers != NULL);

    int res = 0;
    for (int i = start; i < end; ++i)
    {
        res += seq->numbers[i];
    }

    return res;
}

static int sub(const Sequence* const seq, const int start, const int end)
{
    assert(seq != NULL);
    assert(seq->numbers != NULL);

    int res = 0;
    for (int i = start; i < end; ++i)
    {
        res -= seq->numbers[i];
    }

    return res;
}

static int count_chars(const char* const file_path)
{
    int count = 0;
    if (file_path != NULL)
    {
        FILE* fp = fopen(file_path, "r");
        if (fp != NULL)
        {
            char c = fgetc(fp);
            while ((c != '\n') && (!feof(fp)))
            {
                count++;
                c = fgetc(fp);
            }

            fclose(fp);
        }
    }
    return count;
}
