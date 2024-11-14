#pragma once

#include <stdlib.h>

struct ChainedListElement {
	void *element;
	struct ChainedListElement *next;
};

struct ChainedListElement *chained_list_insert(void *element, struct ChainedListElement *parent);
struct ChainedListElement *chained_list_insert_last(void *element, struct ChainedListElement *parent_root);
struct ChainedListElement *chained_list_find_last(struct ChainedListElement *start);

