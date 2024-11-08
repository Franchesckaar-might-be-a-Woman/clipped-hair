#pragma once

#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#include "tree.h"

enum LanguageDefinitionParserState {
	LANGUAGE_DEFINITION_STATE_SKIP = -1,
	LANGUAGE_DEFINITION_STATE_RR_SEARCH = 10,
	LANGUAGE_DEFINITION_STATE_RR_REPLACE = 11,
};

enum LanguageDefinitionCharDefinition {
	LANGUAGE_DEFINITION_CHAR_ZERO = '0',
	LANGUAGE_DEFINITION_CHAR_HEX = 'x',
	LANGUAGE_DEFINITION_CHAR_RR = '/',
};

enum LanguageDefinitionDependencyType {
	LANGUAGE_DEFINITION_DEPENDENCY_BYTE = 1,
	LANGUAGE_DEFINITION_DEPENDENCY_COMMAND = 2,
	LANGUAGE_DEFINITION_DEPENDENCY_MODIFIER_OR = 10,
};

struct LanguageDefinitionSearchReplaceList {
	char *sequence_start;
	char *sequence_end;
	char *search;
	char *replace;
	struct LanguageDefinitionSearchReplaceList *next;
};

struct LanguageDefinitionDependencyList {
	char type;
	char byte;
	struct TreeLeaf *dependency;
	struct LanguageDefinitionDependencyList *next;
};

struct LanguageDefinitionDependencyTree {
	char *identifier;
	struct LanguageDefinitionDependencyTree *children;
	struct LanguageDefinitionDependencyTree *next;
};

struct LanguageDefinitionCompilerOutput {
	char *program;
	int size;
};

struct LanguageDefinitionSearchReplaceList *language_definition_tokenize(char *input_value);
struct TreeLeaf *language_definition_create_command_tree(struct LanguageDefinitionSearchReplaceList *search_replace_list_root);
void language_definition_resolve_command_tree(struct TreeLeaf *command_tree_root);
struct LanguageDefinitionCompilerOutput *language_definition_compile(char *input_program);

