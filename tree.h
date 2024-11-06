#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

struct TreeLeaf {
	struct TreeBranchLinked *links;
	uint32_t identifier;
	void *content;
};

struct TreeBranchLinked {
	struct TreeLeaf *from;
	struct TreeLeaf *to;
	struct TreeBranchLinked *next;
};

struct TreeLeaf *tree_create_root(void);
struct TreeLeaf *tree_insert_leaf(uint32_t identifier, struct TreeLeaf *parent);
struct TreeLeaf *tree_find_child(uint32_t identifier, struct TreeLeaf *parent);

