#include <stdio.h>

#include "language_definition.h"
#include "tree.h"

int main(int argc, char *argv[]) {
	struct LanguageDefinitionCompilerOutput *output;

	FILE *input_file = fopen(argv[1], "r");
	FILE *output_file = fopen(argv[2], "w");
	size_t input_file_length;
	char *input_file_content;

	// Allocate the right size
	fseek(input_file, 0, SEEK_END);
	input_file_length = ftell(input_file);
	rewind(input_file);

	input_file_content = malloc((input_file_length + 1) * sizeof(char));
	fread(input_file_content, input_file_length, 1, input_file);

	output = language_definition_compile(input_file_content);
	fwrite(output->program, output->size * sizeof(char), 1, output_file);

	fclose(input_file);
	fclose(output_file);

	return 0;
}

