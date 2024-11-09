#include "builtin.h"

char char_as_hex(char hex_char) {
	if(hex_char >= 0x30 && hex_char <= 0x39) return hex_char - 0x30;
	else if(hex_char >= 0x41 && hex_char <= 0x46) return (hex_char - 0x37);
	else if(hex_char >= 0x61 && hex_char <= 0x66) return (hex_char - 0x57);
	// Error
	else return -1;
}

struct BuiltinCommand *builtin_command_parse(char *current_char) {
	struct BuiltinCommand *processed_command = malloc(sizeof(struct BuiltinCommand));
	char first_halfbyte, second_halfbyte;

	switch(*current_char) {
		case BUILTIN_CHAR_SAVE_RESTORE:
			processed_command->type = BUILTIN_COMMAND_SAVE_RESTORE;
			processed_command->char_count = 1;
			return processed_command;
		break;

		case BUILTIN_CHAR_MODIFIER_OR:
			processed_command->type = BUILTIN_COMMAND_MODIFIER_OR;
			processed_command->char_count = 5;
			current_char++;
		break;

		case BUILTIN_CHAR_BYTE_PREFIX_ZERO:
			processed_command->type = BUILTIN_COMMAND_BYTE;
			processed_command->char_count = 4;
		break;
	}

	if(*current_char != BUILTIN_CHAR_BYTE_PREFIX_ZERO) return NULL;
	if(*(current_char + 1) != BUILTIN_CHAR_BYTE_PREFIX_X_LOWERCASE) return NULL;

	first_halfbyte = char_as_hex(*(current_char + 2));
	if(first_halfbyte == -1) return NULL;
	second_halfbyte = char_as_hex(*(current_char + 3));
	if(second_halfbyte == -1) return NULL;

	processed_command->byte = (first_halfbyte << 4) + second_halfbyte;
	return processed_command;
}

