#ifndef __DICT_H__
#define __DICT_H__

typedef struct dict_t dict_t;

dict_t* create_default_dict();
dict_t* create_dict(unsigned size);
void free_dict(dict_t* const dict);

char* dict_get(const dict_t* const dict, const char* const key);
void dict_set(const dict_t* const dict, const char* const key, const char* const value);

#endif  // __DICT_H__
