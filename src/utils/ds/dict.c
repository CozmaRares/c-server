#include "dict.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils.h"

#define MAX_DICT_SIZE 10003

struct entry {
    char* key;
    char* value;
    struct entry* next;
};

struct dict_t {
    struct entry** entries;
    unsigned size;
};

struct entry* create_entry(const char* const key, const char* const value) {
    struct entry* e;
    MALLOC(struct entry, e, 1);

    e->key   = new_string(key);
    e->value = new_string(value);
    e->next  = NULL;

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
    CALLOC(struct entry*, dict->entries, size);

    return dict;
}

void free_list(struct entry* e) {
    if (e == NULL)
        return;
    free_list(e->next);
    free(e->key);
    free(e->value);
    free(e);
}

void free_dict(dict_t** const dict) {
    dict_t* d = *dict;
    for (int i = 0; i < d->size; i++)
        if (d->entries[i])
            free_list(d->entries[i]);

    free(d->entries);
    d->entries = NULL;
    d->size    = 0;

    free(d);
    *dict = NULL;
}

int hash(const dict_t* const dict, const char* const key) {
    unsigned long hash = 5381;

    for (int i = 0; key[i]; i++)
        hash = (hash << 5) + hash + key[i];  // hash * 33 + chr

    return hash % dict->size;
}

char* dict_get(const dict_t* const dict, const char* const key) {
    int h = hash(dict, key);

    struct entry* e = dict->entries[h];

    while (e != NULL && strcmp(key, e->key) != 0)
        e = e->next;

    if (e != NULL)
        return new_string(e->value);

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
            e->value = new_string(value);
        }

        prev = e;
        e    = e->next;
    }

    prev->next = create_entry(key, value);
}

void dict_dump(const dict_t* const dict) {
    for (int i = 0; i < dict->size; ++i) {
        struct entry* entry = dict->entries[i];

        if (entry == NULL)
            continue;

        printf("slot[%4d]: ", i);

        while (entry != NULL) {
            printf("%s=%s ", entry->key, entry->value);
            entry = entry->next;
        }

        printf("\n");
    }
}
