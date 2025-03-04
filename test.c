#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "builtin.h"
#include "chained_list.h"
#include "tree.h"
#include "language_definition.h"
#include "address_space.h"

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
	ASSERT_EQUAL("second leaf has no content", ((struct TreeLeaf *) output_tree->children->element)->content, NULL);
	ASSERT_STRING_EQUAL("third leaf contains replace string", ((struct TreeLeaf *) ((struct TreeLeaf *) output_tree->children->element)->children->element)->content, input_list.replace);

	return true;
}

bool test_03_lexer_resolves_from_program(void) {
	printf("TEST	lexer resolves all from program\n");

	char input_program[] = "/no/0x900x80/";
	struct LanguageDefinitionSearchReplaceList *sr_list = language_definition_tokenize(input_program);
	struct TreeLeaf *tree_root = language_definition_create_command_tree(sr_list);
	language_definition_resolve_command_tree(tree_root);

	struct ChainedListElement *command_list = (struct ChainedListElement *) ((struct TreeLeaf *) ((struct TreeLeaf *) tree_root->children->element)->children->element)->content;
	struct BuiltinCommand *first_command = (struct BuiltinCommand *) command_list->element;
	struct BuiltinCommand *second_command = (struct BuiltinCommand *) command_list->next->element;

	ASSERT_EQUAL("first command is byte", first_command->type, BUILTIN_COMMAND_BYTE);
	ASSERT_EQUAL("first command is correct", first_command->byte, (char) 0x90);
	ASSERT_EQUAL("second command is byte", second_command->type, BUILTIN_COMMAND_BYTE);
	ASSERT_EQUAL("second command is correct", second_command->byte, (char) 0x80);
	ASSERT_EQUAL("no third command", command_list->next->next, NULL);

	return true;
}

bool test_10_full_compilation_process(void) {
	printf("TEST	compiler works on basic case\n");

	char input_program[] = "/nop/0x90//mnv/0x53/ nop mnop mnv 0x10";
	struct LanguageDefinitionCompilerOutput *output = language_definition_compile(input_program);

	ASSERT_EQUAL("reports right size", output->size, 4);
	ASSERT_EQUAL("first byte", output->program[0], (char) 0x90);
	ASSERT_EQUAL("second byte", output->program[1], (char) 0x90);
	ASSERT_EQUAL("third byte", output->program[2], (char) 0x53);
	ASSERT_EQUAL("fourth byte", output->program[3], (char) 0x10);

	return true;
}

bool test_11_supports_same_root_commands(void) {
	printf("TEST	compiler supports commands starting alike\n");

	char input_program[] = "/cli/0xfa//cld/0xfc/ cli cld";
	struct LanguageDefinitionCompilerOutput *output = language_definition_compile(input_program);

	ASSERT_EQUAL("reports right size", output->size, 2);
	ASSERT_EQUAL("first byte", output->program[0], (char) 0xFA);
	ASSERT_EQUAL("second byte", output->program[1], (char) 0xFC);
	ASSERT_EQUAL("no more", output->program[2], 0x00);

	return true;
}

bool test_12_supports_modifiers(void) {
	printf("TEST	compiler supports command modifiers\n");

	char input_program[] = "/mov/0x8e//nul/0x00//\%sax/|0x00//\%dds/|0x03/ mov nul \%sax \%dds";
	struct LanguageDefinitionCompilerOutput *output = language_definition_compile(input_program);

	ASSERT_EQUAL("reports right size", output->size, 2);
	ASSERT_EQUAL("first byte", output->program[0], (char) 0x8E);
	ASSERT_EQUAL("second byte", output->program[1], (char) 0x03);

	return true;
}

bool test_20_builtin_byte(void) {
	printf("TEST	builtin byte command works\n");

	char input_value[] = "0x30";
	struct BuiltinCommand *command = builtin_command_parse(input_value);

	ASSERT_EQUAL("command type", command->type, BUILTIN_COMMAND_BYTE);
	ASSERT_EQUAL("char count", command->char_count, 4);
	ASSERT_EQUAL("content", command->byte, 0x30);

	return true;
}

bool test_21_builtin_position(void) {
	printf("TEST	builtin position command works\n");

	char input_value[] = "!";
	struct BuiltinCommand *command = builtin_command_parse(input_value);

	ASSERT_EQUAL("command type", command->type, BUILTIN_COMMAND_SAVE_RESTORE);
	ASSERT_EQUAL("char count", command->char_count, 1);

	return true;
}

bool test_22_builtin_invalid(void) {
	printf("TEST	builtin rejects invalid command\n");

	char input_value[] = " ";
	struct BuiltinCommand *command = builtin_command_parse(input_value);

	ASSERT_EQUAL("is null", command, NULL);

	return true;
}

bool test_30_chained_list_insert(void) {
	printf("TEST	chained list properly inserts children\n");

	struct ChainedListElement *root = (struct ChainedListElement *) malloc(sizeof(struct ChainedListElement));
	struct ChainedListElement *second = chained_list_insert((void *) 0x1, root);
	struct ChainedListElement *third = chained_list_insert((void *) 0x2, second);
	struct ChainedListElement *last = chained_list_insert_last((void *) 0xf, root);

	ASSERT_EQUAL("insert second", root->next->element, (void *) 0x1);
	ASSERT_EQUAL("insert third", root->next->next->element, (void *) 0x2);
	ASSERT_EQUAL("insert last", root->next->next->next->element, (void *) 0xf);

	return true;
}

bool test_31_tree_insert(void) {
	printf("TEST	tree properly inserts children\n");

	struct TreeLeaf *root = tree_create_root();
	struct TreeLeaf *j = tree_insert_leaf((uint32_t) 'j', root);
	struct TreeLeaf *jn = tree_insert_leaf((uint32_t) 'n', j);
	struct TreeLeaf *jm = tree_insert_leaf((uint32_t) 'm', j);

	struct ChainedListElement *root_children = (struct ChainedListElement *) root->children;

	ASSERT_EQUAL("on root - first", ((struct TreeLeaf *) root_children->element)->identifier, (uint32_t) 'j');
	ASSERT_EQUAL("on root - no second", root_children->next, NULL);
	ASSERT_EQUAL("below root - first", ((struct TreeLeaf *) ((struct TreeLeaf *) root_children->element)->children->element)->identifier, (uint32_t) 'n');
	ASSERT_EQUAL("below root - second", ((struct TreeLeaf *) ((struct TreeLeaf *) root_children->element)->children->next->element)->identifier, (uint32_t) 'm');

	return true;
}

bool test_40_address_space_unify(void) {
	printf("TEST	two address spaces can be unified\n");

	int i = 0;
	struct AddressSpace *first_space = address_space_create(0x10, 4);
	for(; i < 4; i++) first_space->bytes[i] = i;
	first_space->byte_counter = 4;
	struct AddressSpace *second_space = address_space_create(0x0, 4);
	for(; i < 8; i++) second_space->bytes[i - 4] = i;
	second_space->byte_counter = 4;
	struct AddressSpace *unified_space = address_space_unify(first_space, second_space);

	for(i = 0; i < 4; i++) ASSERT_EQUAL("byte match", unified_space->bytes[0x10 + i], i);
	for(; i < 8; i++) ASSERT_EQUAL("byte match", unified_space->bytes[i - 4], i);
	ASSERT_EQUAL("size right", unified_space->byte_counter, 0x14);

	return true;
}

int main(void) {
	bool success = true;

	success &= ~test_01_lexer_tokenizes();
	success &= ~test_02_lexer_creates_command_tree();
	success &= ~test_03_lexer_resolves_from_program();

	success &= ~test_10_full_compilation_process();
	success &= ~test_11_supports_same_root_commands();
	success &= ~test_12_supports_modifiers();

	success &= ~test_20_builtin_byte();
	success &= ~test_21_builtin_position();
	success &= ~test_22_builtin_invalid();

	success &= ~test_30_chained_list_insert();
	success &= ~test_31_tree_insert();

	success &= ~test_40_address_space_unify();

	return success;
}
