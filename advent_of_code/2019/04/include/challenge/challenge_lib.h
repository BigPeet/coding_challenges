#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

typedef int (*criteria_f)(int, int, int);

int fits_first_set_criteria(const int passcode, const int lower, const int upper);
int fits_second_set_criteria(const int passcode, const int lower, const int upper);
int num_of_solutions(const int lower_bound, const int upper_bound, const criteria_f critfunc);

int is_in_range(const int passcode, const int lower, const int upper);
int has_six_digits(const int passcode);
int no_decreasing_digits(const int passcode);
int has_repeating_digits(const int passcode);
int has_pair_repeating_digits(const int passcode);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
