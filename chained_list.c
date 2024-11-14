#include "chained_list.h"

struct ChainedListElement *chained_list_insert(void *element, struct ChainedListElement *parent) {
	struct ChainedListElement *child = (struct ChainedListElement *) malloc(sizeof(struct ChainedListElement));

	child->element = element;
	child->next = NULL;

	if(parent != NULL) parent->next = child;

	return child;
}

struct ChainedListElement *chained_list_insert_last(void *element, struct ChainedListElement *parent_root) {
	struct ChainedListElement *parent = chained_list_find_last(parent_root);
	return chained_list_insert(element, parent);
}

struct ChainedListElement *chained_list_find_last(struct ChainedListElement *start) {
	struct ChainedListElement *current = start;
	while(current->next != NULL) {
		current = current->next;
	}

	return current;
}
