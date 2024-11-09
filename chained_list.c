#include "chained_list.h"

struct ChainedListElement *chained_list_insert(void *element, struct ChainedListElement *parent) {
	struct ChainedListElement *child = (struct ChainedListElement *) malloc(sizeof(struct ChainedListElement));

	child->element = element;
	child->next = NULL;

	if(parent != NULL) parent->next = child;

	return child;
}

