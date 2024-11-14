#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "chained_list.h"

struct TreeLeaf {
	struct ChainedListElement *children;
	uint32_t identifier;
	void *content;
};

struct TreeLeaf *tree_create_root(void);
struct TreeLeaf *tree_insert_leaf(uint32_t identifier, struct TreeLeaf *parent);
struct TreeLeaf *tree_find_child(uint32_t identifier, struct TreeLeaf *parent);

