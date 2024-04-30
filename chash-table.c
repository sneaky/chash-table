#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "chash-table.h"

Hashtable* newTable(size_t capacity) {
	Hashtable* ht = (Hashtable*) malloc(sizeof(Hashtable));
	ht->capacity = capacity;
	ht->load = 0;
	ht->loadfactor = 0.0;
	ht->table = calloc(capacity, sizeof(Item));
	return ht;
}

Item* newItem(void* val) {
	unsigned char* s = (unsigned char*) val;
	Item* item = (Item*) malloc(sizeof(Item));
	item->val = s;
	item->count = 0;
	item->key = hash(s);
	item->next = NULL;
	return item;
}

/* djb2 hashing algorithm from http://www.cse.yorku.ca/~oz/hash.html */
size_t hash(unsigned char* str) {
	size_t hash = 5381;
	int c;

	while ((c = *str++)) { // using double paren to get rid of compiler warning
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	}

	return hash;
}

size_t getIndex(size_t key, Hashtable* ht) {
	return (size_t) (key % ht->capacity);
}

void insertItem(Item* item, Hashtable* ht) {
	size_t index = getIndex(item->key, ht);
	item->index = index;

	if (ht->table[index] == NULL) {
		ht->table[index] = item;
		ht->table[index]->count++;
		ht->uniques++;
	} else {
		Item* currentItem = ht->table[index];
		if (strcmp((char*) currentItem->val, (char*) item->val) == 0) {
			free(item);
			currentItem->count++;
		} else {
			while (currentItem->next != NULL && strcmp((char*) currentItem->val, (char*) item->val) != 0) {
				currentItem = currentItem->next;
			}
			if (strcmp((char*) currentItem->val, (char*) item->val) == 0) {
				free(item);
				currentItem->count++;
			} else {
				currentItem->next = item;
				currentItem->next->count++;
			}
		}
	}
	ht->load++;
	ht->loadfactor = (double) ht->load / (double) ht->capacity;
}

void printTable(Hashtable* ht) {
	printf("idx %8c count\tval\n\n", ' ');
	for (int i = 0; i < ht->capacity; i++) {
		if (ht->table[i] != NULL) {
			Item* currentItem = ht->table[i];
			while (currentItem != NULL) {
				printf("%3d %10lu\t\t%s\n", i, currentItem->count, (char*) currentItem->val);
				currentItem = currentItem->next;
			}
		}
	}
}

int compare(const void* a, const void* b) {
	unsigned int ai = (*(Item **)a)->count;
	unsigned int bi = (*(Item **)b)->count;

	return (bi - ai);
}

void freeTable(Hashtable* ht) {
	size_t i = 0;
	Item* current = ht->table[i];
	Item* tmp = NULL;

	while (i < ht->capacity) {
		if (!current) {
			tmp = current;
			current = ht->table[++i];
			free(tmp);
		} else {
			while (current->next) {
				tmp = current;
				current = current->next;
				free(tmp);
			}
			tmp = current;
			current = ht->table[++i];
			free(tmp);
		}
	}
	free(ht->table);
	free(ht);
}

bool contains(void* val, Hashtable* ht) {
	size_t key = hash(val);
	size_t index = getIndex(key, ht);
	Item* current = ht->table[index];
	if (current == NULL) { return false; }
	while (current->next != NULL && strcmp((char*) current->val, (char*) val) != 0) {
		current = current->next;		
	}

	if (strcmp((char*) current->val, (char*) val) != 0) {
		return false;
	}

	return true;
}

bool deleteItem(void* val, Hashtable* ht) {
	size_t key = hash(val);
	if (!contains(val, ht)) {
		printf("Testing: val doesn't exist...\n");
		return false; 
	}
	size_t index = getIndex(key, ht);
	Item* current = ht->table[index];
	
	// if item we want to delete is chained
	if (strcmp((char*) current->val, val) != 0) {
		// get previous item
		Item* prev = NULL;
		while (current->next != NULL && strcmp((char*) current->val, (char*) val) != 0) {
			if (strcmp((char*) current->next->val, (char*) val) == 0) {
				prev = current;
			}
			current = current->next;
		}
		
		current->count--;
		if (current->count == 0) {
			// attach prev to current->next
			prev->next = current->next;
			free(current);
		}
		return true;
	}
	
	// item we want to delete is not chained
	current->count--;
	if (current->count == 0) {
		ht->table[index] = NULL;
		free(ht->table[index]);
	}
	return true;
}

int main() {
	Hashtable* ht = newTable(10);
	insertItem(newItem("apple"), ht);
	insertItem(newItem("apple"), ht);
	insertItem(newItem("turtle"), ht);
	insertItem(newItem("orange"), ht);
	insertItem(newItem("ten"), ht);
	insertItem(newItem("free"), ht);
	insertItem(newItem("true"), ht);
	insertItem(newItem("strange"), ht);
	insertItem(newItem("poop"), ht);
	insertItem(newItem("peep"), ht);
	insertItem(newItem("pop"), ht);
	insertItem(newItem("trash"), ht);
	insertItem(newItem("panda"), ht);
	insertItem(newItem("people"), ht);
	insertItem(newItem("stuff"), ht);
	printf("Testing contains(apple, ht): should be true:%s\t\n", contains("apple", ht) ? "true" : "false");
	printf("Testing contains(snapple, ht): should be false:%s\t\n", contains("snapple", ht) ? "true" : "false");
	printTable(ht);
	bool test1 = deleteItem("apple", ht);
	bool test2 = deleteItem("true", ht);
	bool test3 = deleteItem("totalitarian", ht);
	printf("Testing deleteItem(apple, ht): should be true:%s\t\n", test1 ? "true" : "false");
	printf("Testing deleteItem(true, ht): should be true:%s\t\n", test2 ? "true" : "false");
	printf("Testing deleteItem(totalitarian, ht): should be false:%s\t\n", test3 ? "true" : "false");
	printTable(ht);

	return 0;
}
