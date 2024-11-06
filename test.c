#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "tree.h"
#include "language_definition.h"

#define ASSERT_EQUAL(a, x, y) do {			\
	if(x != y) {							\
		printf("FAIL	%s\n", a);			\
		printf("NOTE	%d != %d\n", x, y);	\
		return false;						\
	}										\
	printf("PASS	%s\n", a);				\
} while(0);

#define ASSERT_STRING_EQUAL(a, x, y) do {	\
	if(strcmp(x, y) != 0) {					\
		printf("FAIL	%s\n", a);			\
		printf("NOTE	%s != %s\n", x, y);	\
		return false;						\
	}										\
	printf("PASS	%s\n", a);				\
} while(0);

bool test_01_lexer_tokenizes(void) {
	printf("TEST	lexer properly tokenizes search-replace\n");

	char input_value[] = "/nop/0x90/";
	struct LanguageDefinitionSearchReplaceList *output_list = language_definition_tokenize(input_value);

	ASSERT_STRING_EQUAL("search is correct", output_list->search, "nop");
	ASSERT_STRING_EQUAL("replace is correct", output_list->replace, "0x90");
	ASSERT_EQUAL("only contains one tokens", output_list->next, NULL);

	return true;
}

bool test_02_lexer_creates_command_tree(void) {
	printf("TEST	lexer creates valid command tree\n");

	struct LanguageDefinitionSearchReplaceList input_list;
	input_list.search = "no";
	input_list.replace = "0x90";
	input_list.next = NULL;

	struct TreeLeaf *output_tree = language_definition_create_command_tree(&input_list);
	ASSERT_EQUAL("first leaf has no content", output_tree->content, NULL);
	ASSERT_EQUAL("second leaf has no content", output_tree->links->to->content, NULL);
	ASSERT_STRING_EQUAL("third leaf contains replace string", output_tree->links->to->links->to->content, input_list.replace);

	return true;
}

bool test_03_lexer_resolves_from_program(void) {
	printf("TEST	lever resolves all from program\n");

	char input_program[] = "/no/0x900x80/";
	struct LanguageDefinitionSearchReplaceList *sr_list = language_definition_tokenize(input_program);
	struct TreeLeaf *tree_root = language_definition_create_command_tree(sr_list);
	language_definition_resolve_command_tree(tree_root);

	ASSERT_EQUAL("resolves to two bytes - first", ((struct LanguageDefinitionDependencyList *) tree_root->links->to->links->to->content)->byte, (char) 0x90);
	ASSERT_EQUAL("resolves to two bytes - second", ((struct LanguageDefinitionDependencyList *) tree_root->links->to->links->to->content)->next->byte, (char) 0x80);
	ASSERT_EQUAL("resolves to two bytes - no third", ((struct LanguageDefinitionDependencyList *) tree_root->links->to->links->to->content)->next->next, NULL);

	return true;
}

bool test_04_full_compilation_process(void) {
	char input_program[] = "/nop/0x90//mnv/0x53/ nop mnop mnv";
	struct LanguageDefinitionCompilerOutput *output = language_definition_compile(input_program);

	ASSERT_EQUAL("reports right size", output->size, 3);
	ASSERT_EQUAL("first byte", output->program[0], (char) 0x90);
	ASSERT_EQUAL("second byte", output->program[1], (char) 0x90);
	ASSERT_EQUAL("third byte", output->program[2], (char) 0x53);
	ASSERT_EQUAL("no more", output->program[3], 0x00);

	return true;
}

int main(void) {
	bool success = true;

	success &= ~test_01_lexer_tokenizes();
	success &= ~test_02_lexer_creates_command_tree();
	success &= ~test_03_lexer_resolves_from_program();
	success &= ~test_04_full_compilation_process();

	return success;
}
