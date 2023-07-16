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

dict_t* create_default_dict();
dict_t* create_dict(unsigned size);
void destroy_dict(dict_t** const dict);
dict_entry_t* dict_get(dict_t* const dict, const char* const key) ;
void dict_set(dict_t* const dict, const char* const key, void* const value, size_t value_size);
void dict_for_each(const dict_t* const dict, void (*callback)(const dict_entry_t* const));

#define DICT_SET_STRING(dict, key, value) \
    dict_set(dict, key, value, strlen(value) + 1);

#endif  // __DICT_H__
