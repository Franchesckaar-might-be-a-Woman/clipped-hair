#pragma once

#include <stdlib.h>

struct ChainedListElement {
	void *element;
	struct ChainedListElement *next;
};

struct ChainedListElement *chained_list_insert(void *element, struct ChainedListElement *parent);

