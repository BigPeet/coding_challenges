/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-17
 *
 */

#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

typedef struct
{
    int* numbers;
    int size;
} Sequence;

Sequence* read_sequence(const char* const file_path);
Sequence* create_sequence(const int size);
Sequence* copy_sequence(const Sequence* const seq);
void destroy_sequence(Sequence* const seq);
void print_sequence(const Sequence* const seq, const int len);

Sequence* get_subsequence(const Sequence* const seq, const int start, const int length);
Sequence* repeat_sequence(const Sequence* const seq, const int amount);
int get_value(const Sequence* const seq);

Sequence* run_phases(const Sequence* const input, const int amount, int skip_first_half);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
