/*
 *
 *  Author: Peter Wolf <pwolf2310@gmail.com>
 *  Date: 2019-12-16
 *
 */

#include "challenge/challenge_lib.h"
#include "stdio.h"

static int count_lines(const char* const file_path);
static void parse_line(const char* const line, Reaction* const output);
static void parse_token(const char* const token, Material* const output);
