#ifndef __DICT_H__
#define __DICT_H__

typedef struct dict_t dict_t;

dict_t* create_default_dict();
dict_t* create_dict(unsigned size);
void destroy_dict(dict_t** const dict);

char* dict_get(dict_t* const dict, const char* const key);
void dict_set(dict_t* const dict, const char* const key, const char* const value);
void dict_dump(const dict_t* const dict);

#endif  // __DICT_H__
