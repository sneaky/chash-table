#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

	while (c = *str++) {
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
			}
		}
	}
	ht->load++;
	ht->loadfactor = (double) ht->load / (double) ht->capacity;
}

void printTable(Hashtable* ht) {
	for (int i = 0; i < ht->capacity; i++) {
		if (ht->table[i] != NULL) {
			Item* currentItem = ht->table[i];
			while (currentItem != NULL) {
				printf("%d %10lu %s\n", i, currentItem->count, (char*) currentItem->val);
				currentItem = currentItem->next;
			}
		}
	}
}

void printTopN(Hashtable* ht, unsigned int n) {
	Item* sortableItemArray[ht->capacity];
	unsigned int arrayIndex = 0;

	for (int i = 0; i < ht->capacity; i++) {
		for (Item* currentElement = ht->table[i]; currentElement != NULL && arrayIndex < ht->capacity; currentElement = currentElement->next) {
			sortableItemArray[arrayIndex] = currentElement;
			arrayIndex++;
		}
	}

	qsort(sortableItemArray, arrayIndex, sizeof(Item*), compare);

	int j = 0;
	while (sortableItemArray[j] != NULL) {
		printf("%10lu %s\n", sortableItemArray[j]->count, (char*) sortableItemArray[j]->val);
		j++;
	}

	FILE* f = fopen("log.txt", "w");
	for (int i = 0; i < arrayIndex; i++) {
		fprintf(f, "%10lu %s\n", sortableItemArray[i]->count, (char*) sortableItemArray[i]->val);
	}
	fclose(f);
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

int main() {
	Hashtable* ht = newTable(12);
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

	printTable(ht);
	
	return 0;
}