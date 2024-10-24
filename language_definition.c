#include "language_definition.h"

struct LanguageDefinitionTokenList *language_definition_tokenize(char *input_value) {
	struct LanguageDefinitionTokenList *list_root = malloc(sizeof(struct LanguageDefinitionTokenList));
	struct LanguageDefinitionTokenList *list_current = list_root;

	char state = LANGUAGE_DEFINITION_STATE_SKIP;
	char current_char;
	int current_index;

	for(current_index = 0; current_index < strlen(input_value); current_index++) {
		current_char = input_value[current_index];

		switch(state) {
			case LANGUAGE_DEFINITION_STATE_SKIP:
				if(current_char == LANGUAGE_DEFINITION_CHAR_ZERO) state = LANGUAGE_DEFINITION_STATE_BYTE_HEX;
				else if(current_char == LANGUAGE_DEFINITION_CHAR_RR) {
					list_current->type = LANGUAGE_DEFINITION_TOKEN_RR_SEARCH;
					list_current->start = current_index + 1;

					state = LANGUAGE_DEFINITION_STATE_RR_SEARCH;
				}
			break;
			
			case LANGUAGE_DEFINITION_STATE_BYTE_HEX:
				// Fill token only when sure that the sequence ("0x") is complete
				if(current_char == LANGUAGE_DEFINITION_CHAR_HEX) {
					list_current->type = LANGUAGE_DEFINITION_TOKEN_BYTE;
					list_current->start = current_index + 1;
					current_index += 2;
					list_current->end = current_index;
					list_current->next = malloc(sizeof(struct LanguageDefinitionTokenList));
					list_current = list_current->next;
				// Otherwise, process again current char
				} else current_index--;

				state = LANGUAGE_DEFINITION_STATE_SKIP;
			break;

			case LANGUAGE_DEFINITION_STATE_RR_SEARCH:
				if(current_char == LANGUAGE_DEFINITION_CHAR_RR) {
					list_current->end = current_index - 1;
					list_current->next = malloc(sizeof(struct LanguageDefinitionTokenList));
					list_current = list_current->next;
					list_current->type = LANGUAGE_DEFINITION_TOKEN_RR_REPLACE;
					list_current->start = current_index + 1;

					state = LANGUAGE_DEFINITION_STATE_RR_REPLACE;
				}
			break;

			case LANGUAGE_DEFINITION_STATE_RR_REPLACE:
				if(current_char == LANGUAGE_DEFINITION_CHAR_RR) {
					list_current->end = current_index - 1;
					list_current->next = malloc(sizeof(struct LanguageDefinitionTokenList));
					list_current = list_current->next;

					state = LANGUAGE_DEFINITION_STATE_SKIP;
				}
			break;
		}
	}

	return list_root;
}
