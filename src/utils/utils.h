#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdbool.h>

#define MALLOC(type, name, size)                                                                                         \
    name = (type*)malloc(((size_t)(size)) * sizeof(type));                                                               \
    if (name == NULL) {                                                                                                  \
        err_n_die("Could not allocate enough memory for " #type ", in file %s, at line: %d\n", __BASE_FILE__, __LINE__); \
    }

#define CALLOC(type, name, size)                                                                                         \
    name = (type*)calloc(((size_t)(size)), sizeof(type));                                                                \
    if (name == NULL) {                                                                                                  \
        err_n_die("Could not allocate enough memory for " #type ", in file %s, at line: %d\n", __BASE_FILE__, __LINE__); \
    }

/**
 * Prints an error message based on the specified format string and arguments,
 * then terminates the program.
 *
 * @param fmt The format string for the error message.
 * @param ... Additional arguments to be formatted according to the format string.
 */
void err_n_die(const char* const fmt, ...);

/**
 * Converts a string representation of a double to a double value.
 *
 * @param str The string to convert.
 * @param dest A pointer to the destination variable to store the converted double value.
 * @param strict If true, strictly checks for a valid double format; otherwise, allows lenient parsing.
 * @return 0 if successful conversion, or a non-zero value if an error occurred.
 */
int to_double(const char* const str, double* const dest, bool strict);

/**
 * Creates a new dynamically allocated string and copies the content from the given string.
 *
 * @param str The string to be duplicated.
 * @return A pointer to the newly allocated string.
 *
 * @note If memory allocation fails, the program terminates
 */
char* new_string(const char* const str);

/**
 * Reads the content of a file and returns it as a dynamically allocated string.
 *
 * @param path The path to the file.
 * @return A pointer to the dynamically allocated string containing the file content,
 *         or NULL if the file cannot be read.
 *
 * @note If memory allocation fails, the program terminates
 */
char* read_file(const char* const path);

#endif  // __UTILS_H__
