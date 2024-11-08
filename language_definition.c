#include "tree.h"
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

char char_as_hex(char hex_char) {
	if(hex_char >= 0x30 && hex_char <= 0x39) return hex_char - 0x30;
	else if(hex_char >= 0x41 && hex_char <= 0x46) return (hex_char - 0x37);
	else if(hex_char >= 0x61 && hex_char <= 0x66) return (hex_char - 0x57);
	// Error
	else return -1;
}

void language_definition_resolve_command_tree(struct TreeLeaf *command_tree_root) {
	// Iterate over all existing commands to resolve `replace` fields
	struct TreeBranchLinked *command_tree_child_root = command_tree_root->links;
	struct TreeBranchLinked *command_tree_child_current;
	struct TreeLeaf *command_tree_current;

	for(command_tree_child_current = command_tree_child_root; command_tree_child_current != NULL; command_tree_child_current = command_tree_child_current->next) {
		command_tree_current = command_tree_child_current->to;
		if(command_tree_current->content != NULL) {
			// At this point, replace is a string, let's convert it to tokens
			char *replace = (char *) command_tree_current->content;
			int current_index;
			char current_char;

			struct LanguageDefinitionDependencyList *dependency_list_root = malloc(sizeof(struct LanguageDefinitionDependencyList));
			struct LanguageDefinitionDependencyList *dependency_list_current = dependency_list_root;
			struct LanguageDefinitionDependencyList *dependency_list_previous;

			for(current_index = 0; current_index < strlen(replace); current_index++) {
				current_char = replace[current_index];

				// Process a byte in hexadecimal representation
				if(current_char == LANGUAGE_DEFINITION_CHAR_ZERO && replace[current_index + 1] == LANGUAGE_DEFINITION_CHAR_HEX) {
					dependency_list_current->type = LANGUAGE_DEFINITION_DEPENDENCY_BYTE;
					dependency_list_current->byte = (char_as_hex(replace[current_index + 2]) << 4) + char_as_hex(replace[current_index + 3]);
					dependency_list_current->next = malloc(sizeof(struct LanguageDefinitionDependencyList));
					dependency_list_previous = dependency_list_current;
					dependency_list_current = dependency_list_current->next;

					current_index += 3;
					continue;
				}

				// Process another command being referenced
				// TODO
			}

			dependency_list_previous->next = NULL;
			command_tree_current->content = dependency_list_root;
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

	char *current_char;
	char *previous_match = input_program;

	for(current_char = input_program; *current_char != '\0'; current_char++) {
		if(*current_char == '\n' || *current_char == ' ') printf(" ");
		else printf("%c", *current_char);

		if(avoid_token_current != NULL && current_char == avoid_token_current->sequence_start) {
			printf(" S%d->", current_char - input_program);
			current_char = avoid_token_current->sequence_end;
			avoid_token_current = avoid_token_current->next;
			printf("%d ", current_char - input_program);
			continue;
		}

		if(tree_leaf_current == tree_leaf_root) previous_match = current_char;
		tree_leaf_current = tree_find_child(*current_char, tree_leaf_current);

		if(tree_leaf_current == NULL) {
			tree_leaf_current = tree_leaf_root;
			current_char = previous_match;
			printf(" %d ", current_char - input_program);
			continue;
		}

		if(tree_leaf_current->content != NULL) {
			printf(" M%d\n", current_char - input_program);
			output_program[output_index++] = ((struct LanguageDefinitionDependencyList *) tree_leaf_current->content)->byte;
			// Dynamic reallocation as program grows
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

