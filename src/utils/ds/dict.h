#ifndef __DICT_H__
#define __DICT_H__

#include <stddef.h>
#include <string.h>

typedef struct dict_entry {
    char* key;
    void* value;
    size_t value_size;
    struct dict_entry* next;
} dict_entry_t;

typedef struct dict_t dict_t;

/**
 * Creates a default dictionary.
 *
 * @return A pointer to the created dictionary.
 */
dict_t* create_default_dict();

/**
 * Creates a dictionary with a specified size.
 *
 * @param size The size of the dictionary.
 * @return A pointer to the created dictionary.
 */
dict_t* create_dict(unsigned size);

/**
 * Frees all associated memory with the dictionary.
 *
 * @param dict A double pointer to the dictionary to be destroyed.
 */
void destroy_dict(dict_t** const dict);

/**
 * Retrieves the value associated with a given key from the dictionary.
 *
 * @param dict A pointer to the dictionary.
 * @param key The key to search for.
 * @return A pointer to the corresponding dictionary entry if found, or NULL otherwise.
 *
 * @note The returned dictionary entry SHOULD NOT be freed.
 */
dict_entry_t* dict_get(dict_t* const dict, const char* const key);

/**
 * Sets a key-value pair in the dictionary.
 *
 * @param dict A pointer to the dictionary.
 * @param key The key to set.
 * @param value A pointer to the value to be stored.
 * @param value_size The size of the value in bytes.
 *                   If it is 0 then just the pointer is copied, useful for storing function pointers.
 */
void dict_set(dict_t* const dict, const char* const key, void* const value, size_t value_size);
#define DICT_SET_STRING(dict, key, value) dict_set(dict, key, value, strlen(value) + 1);

/**
 * Iterates over each entry in the dictionary and applies a callback function.
 *
 * @param dict A pointer to the dictionary.
 * @param callback A function pointer to the callback function to be applied to each entry.
 */
void dict_for_each(const dict_t* const dict, void (*callback)(const dict_entry_t* const));


#endif  // __DICT_H__
