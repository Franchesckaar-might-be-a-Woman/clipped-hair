#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "language_definition.h"

#define ASSERT_EQUAL(a, x, y) do {			\
	if(x != y) {							\
		printf("FAIL	%s\n", a);			\
		printf("NOTE	%d != %d\n", x, y);	\
		return false;						\
	}										\
	printf("PASS	%s\n", a);				\
} while(0);

bool test_01_lexer_byte(void) {
	printf("TEST	lexer properly handles bytes\n");

	char input_value[] = "0x50";
	struct LanguageDefinitionTokenList *output_list = language_definition_tokenize(input_value);

	ASSERT_EQUAL("is a byte", output_list->type, LANGUAGE_DEFINITION_TOKEN_BYTE);
	ASSERT_EQUAL("has valid start", output_list->start, 2);
	ASSERT_EQUAL("has valid end", output_list->end, 3);
	ASSERT_EQUAL("is only", output_list->next, NULL);

	return true;
}

bool test_02_lexer_search_replace(void) {
	printf("TEST	lexer properly handles search-replace\n");

	char input_value[] = "/nop/0x90/";
	struct LanguageDefinitionTokenList *output_list = language_definition_tokenize(input_value);

	ASSERT_EQUAL("first token is search", output_list->type, LANGUAGE_DEFINITION_TOKEN_RR_SEARCH);
	ASSERT_EQUAL("first token has valid start", output_list->start, 1);
	ASSERT_EQUAL("first token has valid end", output_list->end, 3);

	output_list = output_list->next;
	ASSERT_EQUAL("second token is replace", output_list->type, LANGUAGE_DEFINITION_TOKEN_RR_REPLACE);
	ASSERT_EQUAL("second token has valid start", output_list->start, 5);
	ASSERT_EQUAL("second token has valid end", output_list->end, 8);
	ASSERT_EQUAL("only contains two tokens", output_list->next, NULL);

	return true;
}

bool test_03_lexer_tricky_byte(void) {
	printf("TEST	lexer supports tricky byte sequence\n");

	char input_value[] = "000x50";
	struct LanguageDefinitionTokenList *output_list = language_definition_tokenize(input_value);

	ASSERT_EQUAL("is a byte", output_list->type, LANGUAGE_DEFINITION_TOKEN_BYTE);
	ASSERT_EQUAL("has valid start", output_list->start, 4);
	ASSERT_EQUAL("has valid end", output_list->end, 5);
	ASSERT_EQUAL("is only", output_list->next, NULL);

	return true;
}

// To be done: ///nop/0x90/
// To be done: 0/nop/0x90/

int main(void) {
	bool success = true;

	success &= ~test_01_lexer_byte();
	success &= ~test_02_lexer_search_replace();
	success &= ~test_03_lexer_tricky_byte();

	return success;
}
