#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TABLE_SIZE 1000

typedef struct entry_t {
    char *key;
    char *value;
    struct entry_t *next;
} entry_t;

typedef struct {
    entry_t **entries; // read as: array of pointer
} ht_t;

ht_t *ht_create(void);
void ht_set(ht_t *hashtable, const char *key, const char *value);
char *ht_get(ht_t *hashtable, const char *key);

#endif