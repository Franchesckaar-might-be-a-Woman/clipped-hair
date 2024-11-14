#include "language_definition.h"

struct LanguageDefinitionSearchReplaceList *language_definition_tokenize(char *input_value) {
	struct LanguageDefinitionSearchReplaceList *list_root = malloc(sizeof(struct LanguageDefinitionSearchReplaceList));
	struct LanguageDefinitionSearchReplaceList *list_current = list_root;
	struct LanguageDefinitionSearchReplaceList *list_previous = NULL;

	char *search_replace_start;
	char *search_replace_end;
	int search_replace_length;

	char state = LANGUAGE_DEFINITION_STATE_SKIP;
	char *current_char;

	for(current_char = input_value; *current_char != '\0'; current_char++) {
		switch(state) {
			case LANGUAGE_DEFINITION_STATE_SKIP:
				if(*current_char == LANGUAGE_DEFINITION_CHAR_RR) {
					search_replace_start = current_char + 1;
					list_current->sequence_start = current_char;
					state = LANGUAGE_DEFINITION_STATE_RR_SEARCH;
				}
			break;

			case LANGUAGE_DEFINITION_STATE_RR_SEARCH:
				if(*current_char == LANGUAGE_DEFINITION_CHAR_RR) {
					search_replace_end = current_char;
					search_replace_length = search_replace_end - search_replace_start;
					list_current->search = malloc(search_replace_length * sizeof(char));
					memcpy(list_current->search, search_replace_start, search_replace_length);
					list_current->search[search_replace_length] = '\0';
					search_replace_start = current_char + 1;

					state = LANGUAGE_DEFINITION_STATE_RR_REPLACE;
				}
			break;

			case LANGUAGE_DEFINITION_STATE_RR_REPLACE:
				if(*current_char == LANGUAGE_DEFINITION_CHAR_RR) {
					search_replace_end = current_char;
					search_replace_length = search_replace_end - search_replace_start;
					list_current->replace = malloc(search_replace_length * sizeof(char));
					memcpy(list_current->replace, search_replace_start, search_replace_length);
					list_current->replace[search_replace_length] = '\0';
					list_current->sequence_end = current_char;

					list_current->next = malloc(sizeof(struct LanguageDefinitionSearchReplaceList));
					list_previous = list_current;
					list_current = list_current->next;

					state = LANGUAGE_DEFINITION_STATE_SKIP;
				}
			break;
		}
	}

	if(list_previous != NULL) list_previous->next = NULL;

	return list_root;
}

struct TreeLeaf *language_definition_create_command_tree(struct LanguageDefinitionSearchReplaceList *search_replace_list_root) {
	struct LanguageDefinitionSearchReplaceList *search_replace_list_current;

	struct TreeLeaf *command_tree_root = malloc(sizeof(struct TreeLeaf));
	struct TreeLeaf *command_tree_current = command_tree_root;
	struct TreeLeaf *command_tree_found;

	int current_index;
	char current_char;

	for(search_replace_list_current = search_replace_list_root; search_replace_list_current != NULL; search_replace_list_current = search_replace_list_current->next) {
		for(current_index = 0; current_index < strlen(search_replace_list_current->search); current_index++) {
			current_char = search_replace_list_current->search[current_index];
			// Unify tree leaves, since tree library does not assumes identifiers to be unique
			command_tree_found = tree_find_child((uint32_t) current_char, command_tree_current);
			if(command_tree_found != NULL) command_tree_current = command_tree_found;
			else command_tree_current = tree_insert_leaf((uint32_t) current_char, command_tree_current);
		}

		command_tree_current->content = (void *) search_replace_list_current->replace;
		command_tree_current = command_tree_root;
	}

	return command_tree_root;
}

void language_definition_resolve_command_tree(struct TreeLeaf *command_tree_root) {
	// Iterate over all existing commands to resolve `replace` fields
	struct ChainedListElement *command_tree_child_root = command_tree_root->children;
	struct ChainedListElement *command_tree_child_current;
	struct TreeLeaf *command_tree_current;

	for(command_tree_child_current = command_tree_child_root; command_tree_child_current != NULL; command_tree_child_current = command_tree_child_current->next) {
		command_tree_current = (struct TreeLeaf *) command_tree_child_current->element;
		if(command_tree_current->content != NULL) {
			// At this point, replace is a string, let's convert it to tokens
			char *replace = (char *) command_tree_current->content;
			char *current_char;

			struct ChainedListElement *command_list_current = NULL;
			struct ChainedListElement *command_list_root = NULL;
			struct BuiltinCommand *command_current = NULL;

			for(current_char = replace; *current_char != '\0'; current_char++) {
				command_current = builtin_command_parse(current_char);
				if(command_current == NULL) continue;

				current_char += command_current->char_count - 1;
				command_list_current = chained_list_insert((void *) command_current, command_list_current);
				if(command_list_root == NULL) command_list_root = command_list_current;
			}

			command_tree_current->content = (void *) command_list_root;
		}

		language_definition_resolve_command_tree(command_tree_current);
	}
}

struct LanguageDefinitionCompilerOutput *language_definition_compile(char *input_program) {
	int output_size = 512;
	int output_index = 0;
	char *output_program = malloc(output_size * sizeof(char));

	struct LanguageDefinitionCompilerOutput *output_struct = malloc(sizeof(struct LanguageDefinitionCompilerOutput));

	struct LanguageDefinitionSearchReplaceList *avoid_token_root = language_definition_tokenize(input_program);
	struct LanguageDefinitionSearchReplaceList *avoid_token_current = avoid_token_root;

	struct TreeLeaf *tree_leaf_root = language_definition_create_command_tree(avoid_token_root);
	struct TreeLeaf *tree_leaf_current = tree_leaf_root;
	language_definition_resolve_command_tree(tree_leaf_root);

	struct BuiltinCommand *command_current = NULL;
	struct ChainedListElement *tree_leaf_command_list;

	char *current_char;
	char *previous_match = input_program;
	bool has_extra_output;

	for(current_char = input_program; *current_char != '\0'; current_char++) {
		has_extra_output = false;

		// Debug: show all chars in processing order
		if(*current_char == '\n' || *current_char == ' ') printf(" ");
		else printf("%c", *current_char);

		// Skip search-replace sequences
		if(avoid_token_current != NULL && current_char == avoid_token_current->sequence_start) {
			printf(" S%d->", current_char - input_program);
			current_char = avoid_token_current->sequence_end;
			avoid_token_current = avoid_token_current->next;
			printf("%d ", current_char - input_program);
			continue;
		}

		// Process builtins
		// XXX: support other commands, by refactoring with what's below
		command_current = builtin_command_parse(current_char);
		if(command_current != NULL && command_current->type == BUILTIN_COMMAND_BYTE) {
			printf(" MB%d\n", current_char - input_program);
			output_program[output_index++] = command_current->byte;
			current_char += command_current->char_count - 1;
			has_extra_output = true;
		}

		// Advance in tree by current char
		if(tree_leaf_current == tree_leaf_root) previous_match = current_char;
		tree_leaf_current = tree_find_child(*current_char, tree_leaf_current);

		if(tree_leaf_current == NULL) {
			tree_leaf_current = tree_leaf_root;
			current_char = previous_match;
			printf(" %d ", current_char - input_program);
		}

		// Append current content from tree
		if(tree_leaf_current->content != NULL) {
			printf(" MC%d\n", current_char - input_program);
			tree_leaf_command_list = (struct ChainedListElement *) tree_leaf_current->content;

			while(tree_leaf_command_list != NULL) {
				command_current = (struct BuiltinCommand *) tree_leaf_command_list->element;
				switch(command_current->type) {
					case BUILTIN_COMMAND_BYTE:
						output_program[output_index++] = command_current->byte;
						break;

					case BUILTIN_COMMAND_MODIFIER_OR:
						output_program[output_index - 1] |= command_current->byte;
						break;
				}

				tree_leaf_command_list = tree_leaf_command_list->next;
			}

			has_extra_output = true;
		}

		// Dynamic reallocation as program grows
		if(has_extra_output) {
			if(output_index >= output_size) {
				output_size = output_size * 4;
				output_program = realloc(output_program, output_size * sizeof(char));
			}
			previous_match = current_char;
		}
	}

	printf("\n");

	output_struct->program = output_program;
	output_struct->size = output_index;
	return output_struct;
}

