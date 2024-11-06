#include "tree.h"

struct TreeLeaf *tree_create_root(void) {
	struct TreeLeaf *root = malloc(sizeof(struct TreeLeaf));
	root->identifier = 0;
	root->links = NULL;
	root->content = NULL;

	return root;
}

struct TreeLeaf *tree_insert_leaf(uint32_t identifier, struct TreeLeaf *parent) {
	struct TreeLeaf *leaf = malloc(sizeof(struct TreeLeaf));
	leaf->identifier = identifier;
	leaf->links = NULL;
	leaf->content = NULL;

	struct TreeBranchLinked *parent_new_child = parent->links;

	if(parent_new_child == NULL) {
		parent->links = malloc(sizeof(struct TreeBranchLinked));
		parent_new_child = parent->links;
	} else {
		while(parent_new_child->next != NULL) {
			parent_new_child = parent_new_child->next;
		}

		parent_new_child->next = malloc(sizeof(struct TreeBranchLinked));
		parent_new_child = parent_new_child->next;
	}

	parent_new_child->from = parent;
	parent_new_child->to = leaf;
	parent_new_child->next = NULL;

	return leaf;
}

struct TreeLeaf *tree_find_child(uint32_t identifier, struct TreeLeaf *parent) {
	struct TreeBranchLinked *branch = parent->links;
	if(branch == NULL) return NULL;

	while(branch->to->identifier != identifier) {
		if(branch->next == NULL) return NULL;
		branch = branch->next;
	}

	return branch->to;
}
