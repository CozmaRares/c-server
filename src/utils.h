#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>

void err_n_die(const char* const fmt, ...);
int to_double(const char* const str, double* const dest, bool strict);

#endif  // __UTILS_H__
