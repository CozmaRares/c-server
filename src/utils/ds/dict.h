#ifndef __DICT_H__
#define __DICT_H__

// TODO: change value to void*
typedef struct dict_entry {
    char* key;
    char* value;
    struct dict_entry* next;
} dict_entry_t;
typedef struct dict_t dict_t;

dict_t* create_default_dict();
dict_t* create_dict(unsigned size);
void destroy_dict(dict_t** const dict);

char* dict_get(dict_t* const dict, const char* const key);
void dict_set(dict_t* const dict, const char* const key, const char* const value);
void dict_for_each(const dict_t* const dict, void (*callback)(const dict_entry_t* const));

#endif  // __DICT_H__
