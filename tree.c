#include "tree.h"

struct TreeLeaf *tree_create_root(void) {
	struct TreeLeaf *root = malloc(sizeof(struct TreeLeaf));
	root->identifier = 0;
	root->children = NULL;
	root->content = NULL;

	return root;
}

struct TreeLeaf *tree_insert_leaf(uint32_t identifier, struct TreeLeaf *parent) {
	struct TreeLeaf *leaf = tree_create_root();
	leaf->identifier = identifier;

	if(parent->children == NULL) {
		parent->children = (struct ChainedListElement *) malloc(sizeof(struct ChainedListElement));
		parent->children->element = (void *) leaf;
		parent->children->next = NULL;
	} else chained_list_insert_last((void *) leaf, parent->children);

	return leaf;
}

struct TreeLeaf *tree_find_child(uint32_t identifier, struct TreeLeaf *parent) {
	struct ChainedListElement *branch = parent->children;
	if(branch == NULL) return NULL;

	while(((struct TreeLeaf *) branch->element)->identifier != identifier) {
		if(branch->next == NULL) return NULL;
		branch = branch->next;
	}

	return (struct TreeLeaf *) branch->element;
}
