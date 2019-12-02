#ifndef INCLUDE_CHALLENGE_LIB_H
#define INCLUDE_CHALLENGE_LIB_H

#include "stdlib.h"

size_t* read_modules(const char* const file_path, size_t* const amount_modules);
size_t total_fuel(const size_t* const module_masses, const size_t num_of_modules);
size_t fuel_by_mass(const size_t mass);

size_t total_fuel_complete(const size_t* const module_masses, const size_t num_of_modules);
size_t fuel_for_module(const size_t module_mass);

#endif /* ifndef INCLUDE_CHALLENGE_LIB_H */
