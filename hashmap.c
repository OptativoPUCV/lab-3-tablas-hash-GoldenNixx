#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "hashmap.h"

typedef struct HashMap HashMap;
int enlarge_called = 0;

struct HashMap {
    Pair ** buckets;
    long size;
    long capacity;
    long current;
};

Pair * createPair( char * key,  void * value) {
    Pair * new = (Pair *)malloc(sizeof(Pair));
    new->key = key;
    new->value = value;
    return new;
}

long hash( char * key, long capacity) {
    unsigned long hash_val = 0;
    char * ptr;
    for (ptr = key; *ptr != '\0'; ptr++) {
        hash_val += hash_val*32 + tolower(*ptr);
    }
    return hash_val % capacity;
}

int is_equal(void* key1, void* key2){
    if(key1==NULL || key2==NULL) return 0;
    if(strcmp((char*)key1,(char*)key2) == 0) return 1;
    return 0;
}


HashMap * createMap(long capacity) {
    HashMap * map = (HashMap *) malloc(sizeof(HashMap));
    map->buckets = (Pair **) calloc(capacity, sizeof(Pair *));
    map->capacity = capacity;
    map->size = 0;
    map->current = -1;
    return map;
}


void enlarge(HashMap * map) {
    enlarge_called = 1; //no borrar (testing purposes)

    Pair ** old_buckets = map->buckets;
    long old_capacity = map->capacity;

    map->capacity *= 2;
    map->buckets = (Pair **) calloc(map->capacity, sizeof(Pair *));
    map->size = 0;

    for (long i = 0; i < old_capacity; i++) {
        if (old_buckets[i] != NULL && old_buckets[i]->key != NULL) {
            insertMap(map, old_buckets[i]->key, old_buckets[i]->value);
            free(old_buckets[i]);
        }
    }
    free(old_buckets);
}


void insertMap(HashMap * map, char * key, void * value) {
    if (map->size >= map->capacity * 0.7) {
        enlarge(map);
    }

    long pos = hash(key, map->capacity);

    while (map->buckets[pos] != NULL && map->buckets[pos]->key != NULL) {
        if (is_equal(map->buckets[pos]->key, key)) {
            return;
        }
        pos = (pos + 1) % map->capacity;
    }

    map->buckets[pos] = createPair(key, value);
    map->size++;
    map->current = pos;
}


Pair * searchMap(HashMap * map,  char * key) {
    long pos = hash(key, map->capacity);
    long initial_pos = pos;

    while (map->buckets[pos] != NULL) {
        if (map->buckets[pos]->key != NULL && is_equal(map->buckets[pos]->key, key)) {
            map->current = pos;
            return map->buckets[pos];
        }
        pos = (pos + 1) % map->capacity;
        if (pos == initial_pos) {
            return NULL;
        }
    }
    return NULL;
}


void eraseMap(HashMap * map,  char * key) {
    Pair * pair_to_erase = searchMap(map, key);
    if (pair_to_erase != NULL) {
        pair_to_erase->key = NULL;
        map->size--;
    }
}


Pair * firstMap(HashMap * map) {
    for (long i = 0; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}

Pair * nextMap(HashMap * map) {
    for (long i = map->current + 1; i < map->capacity; i++) {
        if (map->buckets[i] != NULL && map->buckets[i]->key != NULL) {
            map->current = i;
            return map->buckets[i];
        }
    }
    return NULL;
}