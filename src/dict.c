#include "dict.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

#define MAX_DICT_SIZE 10003

struct entry {
    char* key;
    char* value;
    struct entry* next;
};

struct entry* create_entry(const char* const key, const char* const value) {
    struct entry* e = malloc(sizeof(struct entry));

    size_t key_len   = strlen(key);
    size_t value_len = strlen(value);

    e->key   = malloc(key_len + 1);
    e->value = malloc(value_len + 1);

    if (e->key == NULL || e->value == NULL)
        err_n_die("Not enough memory to create dict entry");

    e->next = NULL;

    memcpy(e->key, key, key_len);
    memcpy(e->value, value, value_len);
    e->key[key_len]   = '\0';
    e->key[value_len] = '\0';

    return e;
}

struct dict_t {
    struct entry** entries;
    unsigned size;
};

dict_t* create_default_dict() {
    return create_dict(MAX_DICT_SIZE);
}
dict_t* create_dict(unsigned size) {
    dict_t* dict = malloc(sizeof(dict_t));

    if (dict == NULL)
        err_n_die("Not enough memory for dict creation");

    dict->size    = size;
    dict->entries = calloc(size, sizeof(struct entry*));

    if (dict->entries == NULL)
        err_n_die("Not enough memory for dict creation");

    return dict;
}

void free_list(struct entry* e) {
    if (e->next)
        free_list(e->next);
    free(e);
}

void free_dict(dict_t* const dict) {
    for (int i = 0; i < dict->size; i++)
        if (dict->entries[i])
            free_list(dict->entries[i]);

    free(dict->entries);
    dict->entries = NULL;
    dict->size    = 0;
}

int hash(const dict_t* const dict, const char* const key) {
    unsigned long hash = 5381;

    for (int i = 0; key[i]; i++)
        hash = ((hash << 5) + hash) + key[i];  // hash * 33 + chr

    return hash % dict->size;
}

char* dict_get(const dict_t* const dict, const char* const key) {
    int h = hash(dict, key);

    struct entry* e = dict->entries[h];

    while (e != NULL && strcmp(key, e->key) != 0)
        e = e->next;

    if (e != NULL)
        return e->value;

    return NULL;
}
void dict_set(const dict_t* const dict, const char* const key, const char* const value) {
    int h = hash(dict, key);

    struct entry* e = dict->entries[h];
    struct entry* prev;

    if (e == NULL) {
        dict->entries[h] = create_entry(key, value);
        return;
    }

    while (e) {
        if (strcmp(key, e->key) == 0) {
            free(e->value);

            size_t value_len = strlen(value);
            e->value         = malloc(value_len + 1);

            if (e->value == NULL)
                err_n_die("Not enough memory to create dict entry");

            e->next = NULL;

            memcpy(e->value, value, value_len);
            e->key[value_len] = '\0';
        }

        prev = e;
        e    = e->next;
    }

    prev->next = create_entry(key, value);
}
