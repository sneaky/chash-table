#ifndef CHASH_TABLE_H
#define CHASH_TABLE_H

#include <stddef.h>

typedef struct item {
	void* val;
	size_t key;
	size_t index;
	size_t count;
	struct item* next;
} Item;

typedef struct hashtable {
	Item** table;
	size_t capacity;
	size_t load;
	size_t uniques;
	double loadfactor;
} Hashtable;

size_t hash(unsigned char* s);
Hashtable* newTable(size_t capacity);
Item* newItem(void* val);
size_t getIndex(size_t key, Hashtable* ht);
void insertItem(Item* item, Hashtable* ht);
void printTable(Hashtable* ht);
int compare(const void* a, const void* b);
void freeTable(Hashtable* ht);
bool contains(void* val, Hashtable* ht);
//void deleteItem(size_t key, Hashtable* ht);

#endif
