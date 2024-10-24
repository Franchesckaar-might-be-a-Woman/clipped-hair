#include <string.h>

#include "tree.h"

enum LanguageDefinitionParserState {
	LANGUAGE_DEFINITION_STATE_SKIP = -1,
	LANGUAGE_DEFINITION_STATE_BYTE_HEX = 1,
	LANGUAGE_DEFINITION_STATE_RR_SEARCH = 10,
	LANGUAGE_DEFINITION_STATE_RR_REPLACE = 11,
};

enum LanguageDefinitionTokenType {
	LANGUAGE_DEFINITION_TOKEN_BYTE = 0,
	LANGUAGE_DEFINITION_TOKEN_RR_SEARCH = 10,
	LANGUAGE_DEFINITION_TOKEN_RR_REPLACE = 11,
};

enum LanguageDefinitionCharDefinition {
	LANGUAGE_DEFINITION_CHAR_ZERO = '0',
	LANGUAGE_DEFINITION_CHAR_HEX = 'x',
	LANGUAGE_DEFINITION_CHAR_RR = '/',
};

struct LanguageDefinitionTokenList {
	char type;
	int start;
	int end;
	struct LanguageDefinitionTokenList *next;
};

struct LanguageDefinitionTokenList *language_definition_tokenize(char *input_value);
