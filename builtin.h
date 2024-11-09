#pragma once

#include <stdio.h>
#include <stdlib.h>

enum BuiltinCharDefinition {
	BUILTIN_CHAR_BYTE_PREFIX_ZERO = '0',
	BUILTIN_CHAR_BYTE_PREFIX_X_LOWERCASE = 'x',
	BUILTIN_CHAR_SAVE_RESTORE = '!',
	BUILTIN_CHAR_MODIFIER_OR = '|',
};

enum BuiltinCommandType {
	BUILTIN_COMMAND_BYTE = 1,
	BUILTIN_COMMAND_COMMAND = 2,
	BUILTIN_COMMAND_SAVE_RESTORE = 3,
	BUILTIN_COMMAND_MODIFIER_OR = 10,
};

struct BuiltinCommand {
	char type;
	int char_count;
	char byte;
};

struct BuiltinCommand *builtin_command_parse(char *current_char);
