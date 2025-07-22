#include "defs.h"

// next_char() a function that skips spaces and delimiters in a string for read_types() function

int next_char()
{
	register char* s;
	// checking the end of the file
	if (line == 0)
		return (EOF);
	s = cptr;
	// skiping spaces and delimiters
	for (;;) {
		switch (*s) {
		case ' ':
		case '\t':
		case '\f':
		case '\r':
		case '\v':
		case ',':
		case ';':
			++s;
			break;
		default:
			cptr = s;
			return (*s);
		}
	}
}



char first_open_conflict_file = 0;		// <- new
int conflict_count = 0;					// <- new

void write_conflicts(char* symbol, int ruleno)
{
	FILE* ffile;
	if (!first_open_conflict_file)
	{
		ffile = fopen("conflicts_list.txt", "w");
		first_open_conflict_file = 1;
	}
	else
	{
		ffile = fopen("conflicts_list.txt", "a");
	}
	if (ffile == NULL) {
		error(lineno, 0, 0, "Cannot open conflicts_list file for writing %s", "conflicts_list.txt");
	}
	else {
		fprintf(ffile, "%d:%s conflict on line %d\n", ++conflict_count, symbol, rule_line[ruleno]);
		fclose(ffile);
	}
}

void unexpected_endline() {
	error(lineno, 0, 0, "unexpected end-of-line");
	exit(1);
}

void return_err() {
	error(lineno, 0, 0, "return operator found");
	exit(1);
}

void double_name_err() {
	error(lineno, 0, 0, "repeating the type name");
	exit(1);
}

void MOD_check_additinal_cases(char **s, int *i, int argc, char** argv) {
	switch (**s) {
	case 'c':
		++(*s);
		if (**s)
			c_output_file_name = s;
		else if (++(*i) < argc)
			c_output_file_name = argv[*i];
		else
			usage();
		break;

	case 'h':
		++(*s);
		if (**s)
			h_output_file_name = s;
		else if (++(*i) < argc)
			h_output_file_name = argv[*i];
		else
			usage();
		break;

	default:
		usage();
	}
	return;
}

void MOD_declare_c_file(int *len, char *file_prefix) {
	if (c_output_file_name) {
		*len = strlen(c_output_file_name);

		output_file_name = MALLOC(*len + 1);
		if (output_file_name == 0)
			no_space();
		strcpy(output_file_name, c_output_file_name);
	}
	else {
		*len = strlen(file_prefix);

		output_file_name = MALLOC(*len + 7);
		if (output_file_name == 0)
			no_space();
		strcpy(output_file_name, file_prefix);
		strcpy(output_file_name + *len, OUTPUT_SUFFIX);
	}
}

void MOD_declare_h_file(int *len, char *file_prefix) {
	if (h_output_file_name) {
		*len = strlen(h_output_file_name);

		defines_file_name = MALLOC(*len + 1);
		if (defines_file_name == 0)
			no_space();
		strcpy(defines_file_name, h_output_file_name);
	}
	else {
		*len = strlen(file_prefix);

		defines_file_name = MALLOC(*len + 7);
		if (defines_file_name == 0)
			no_space();
		strcpy(defines_file_name, file_prefix);
		strcpy(defines_file_name + *len, DEFINES_SUFFIX);
	}
}
