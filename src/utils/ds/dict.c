#include "dict.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils.h"

#define MAX_DICT_SIZE 10003

struct dict_t {
    dict_entry_t** entries;
    unsigned size;
};

dict_entry_t* create_entry(const char* const key, void* const value, size_t value_size) {
    dict_entry_t* e;
    MALLOC(dict_entry_t, e, 1);

    e->key        = new_string(key);
    e->value_size = value_size;
    e->next       = NULL;

    if (value_size) {
        MALLOC(void, e->value, value_size);
        memcpy(e->value, value, value_size);
    } else
        e->value = value;

    return e;
}

dict_t* create_default_dict() {
    return create_dict(MAX_DICT_SIZE);
}
dict_t* create_dict(unsigned size) {
    dict_t* dict;

    if (size > MAX_DICT_SIZE)
        size = MAX_DICT_SIZE;

    MALLOC(dict_t, dict, 1);
    dict->size = size;
    CALLOC(dict_entry_t*, dict->entries, size);

    return dict;
}

void free_list(dict_entry_t* e) {
    if (e == NULL)
        return;
    free_list(e->next);
    free(e->key);
    if (e->value_size)
        free(e->value);
    free(e);
}

void destroy_dict(dict_t** const dict) {
    dict_t* d = *dict;
    for (int i = 0; i < d->size; i++)
        if (d->entries[i])
            free_list(d->entries[i]);

    free(d->entries);
    free(d);
    *dict = NULL;
}

int hash(const dict_t* const dict, const char* const key) {
    unsigned long hash = 5381;

    for (int i = 0; key[i]; i++)
        hash = (hash << 5) + hash + key[i];  // hash * 33 + chr

    return hash % dict->size;
}

void* dict_get(dict_t* const dict, const char* const key) {
    int h = hash(dict, key);

    dict_entry_t* e = dict->entries[h];

    while (e != NULL && strcmp(key, e->key) != 0)
        e = e->next;

    if (e != NULL)
        return e->value;

    return NULL;
}

void dict_set(dict_t* const dict, const char* const key, void* const value, size_t value_size) {
    int h = hash(dict, key);

    dict_entry_t* e = dict->entries[h];
    dict_entry_t* prev;

    if (e == NULL) {
        dict->entries[h] = create_entry(key, value, value_size);
        return;
    }

    while (e) {
        if (strcmp(key, e->key) == 0) {
            if (e->value_size)
                free(e->value);

            e->value_size = value_size;

            if (value_size) {
                MALLOC(void, e->value, value_size);
                memcpy(e->value, value, value_size);
            } else
                e->value = value;
        }

        prev = e;
        e    = e->next;
    }

    prev->next = create_entry(key, value, value_size);
}

void dict_for_each(const dict_t* const dict, void (*callback)(const dict_entry_t* const)) {
    for (int i = 0; i < dict->size; i++) {
        dict_entry_t* entry = dict->entries[i];

        if (entry == NULL)
            continue;

        while (entry) {
            callback(entry);
            entry = entry->next;
        }
    }
}
