#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stdlib.h"

/**
 * Nonlinear solution: Will take way longer with increasing input size
 *
 * Remove negative numbers from array (e.g. set to 0)
 * Iterate over numbers to find pos. min:
 *    if found min == 1
 *        decrease array by 1
 * Return the number of iterations necessary
 */
int nonlinear_solution(int* const numbers, const size_t amount);

/**
 * Nonconstant space: Will require more memory with increasing input size
 *
 * Remove negative numbers from array (e.g. set to 0)
 * Find maximum (or use amount) and create array 'found' of that size
 * Iterate over numbers:
 *    set found[numbers[index] - 1] to true
 * Return first index in found that contains false
 */
int nonconstant_solution(int* const numbers, const size_t amount);

/**
 * Remove negative numbers from array (e.g. set to 0)
 * Remove numbers higher than the amount (e.g. set to 0)
 *
 * Now you have an array with numbers within [0, amount].
 * Sort the array by placing each number at its corresponding index.
 * Each entry is moved exactly once at most and only one iteration is necessary.
 * (see 'replace_sort' function)
 * Iterate over the array and return the first index with 0 (or the 'delete' value of your choice).
 */
int linear_constant_solution(int* const numbers, const size_t amount);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
