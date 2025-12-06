#include "hashtable.h"

static unsigned int hash(const char *key) 
{
    unsigned long int value = 0;
    unsigned int key_len = strlen(key);

    // hash formula
    for(int i = 0; i < key_len; i++) {
        value = value * 37 + key[i];
    }

    // making sure value is not > TABLE_SIZE
    value = value % TABLE_SIZE;

    return value;
}

ht_t *ht_create(void) 
{
    // allocate table
    ht_t *hashtable = malloc(sizeof(ht_t) * 1);

    // allocate table entries and set entries to 0
    hashtable->entries = calloc(TABLE_SIZE, sizeof(entry_t *));

    return hashtable;
}

static entry_t *ht_pair(const char *key, const char *value)
{
    // allocate new temp entry
    entry_t *entry = malloc(sizeof(entry_t) * 1);
    entry->key = malloc(strlen(key) + 1);
    entry->value = malloc(strlen(value) + 1);

    // copying key and values
    strcpy(entry->key, key);
    strcpy(entry->value, value);

    // next starts at null but might be assigned later to handle collision
    entry->next = NULL;

    return entry;
}

void ht_set(ht_t *hashtable, const char *key, const char *value)
{
    unsigned int slot = hash(key);

    // attempting to find key
    entry_t *entry = hashtable->entries[slot];

    // if entry is null, insert immedietly
    if(!entry) {
        hashtable->entries[slot] = ht_pair(key, value);
        return;
    }

    entry_t *prev;
    // collision handling
    // walk through each entry until either the end is
    // reached or a matching key is found
    while(entry != NULL)
    {
        if(strcmp(entry->key, key) == 0) {
            free(entry->value);
            entry->value = malloc(strlen(value) + 1);
            strcpy(entry->value, value);
            return;
        }

        // save pointer to previous entry
        // for when the while loop stop and found the null
        prev = entry;
        // walk to next
        entry = prev->next;
    }
    
    // when the while loop ends which tranverse *next
    // which also means the *next is null
    // new data is added
    prev->next = ht_pair(key, value);
}

char *ht_get(ht_t *hashtable, const char *key)
{
    unsigned int slot = hash(key);

    // trying to find the valid slot
    entry_t *entry = hashtable->entries[slot];

    if(!entry) return NULL;

    while(entry != NULL) {
        if(strcmp(key, entry->key) == 0) {
            return entry->value;
        }

        // proceed to next (if available)
        entry = entry->next;
    }

    return NULL;
}

void ht_del(ht_t *hashtable, const char *key)
{
    unsigned int bucket = hash(key);

    entry_t *entry = hashtable->entries[bucket];

    if(!entry) return;

    entry_t *prev;
    int idx = 0;
    while(entry != NULL)
    {
        if(strcmp(key, entry->key) == 0) 
        {
            // first item and no next entry
            if(entry->next == NULL && idx == 0) 
            {
                hashtable->entries[bucket] = NULL;
            }
            // first item with a next entry
            if(entry->next != NULL && idx == 0) 
            {
                hashtable->entries[bucket] = entry->next;
            }
            // middle item
            if(entry->next != NULL && idx != 0) 
            {
                prev->next = entry->next;
            }
            // last item
            if (entry->next == NULL && idx != 0)
            {
                prev->next = NULL;
            }

            // free deleted entries
            free(entry->key);
            free(entry->value);
            free(entry);
            
            return;
        }

        // walk to next
        // so entry is to point next entry
        // while prev is previous
        prev = entry;
        entry = prev->next;
        idx++;
    }
    
}