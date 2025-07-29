#include "defs.h"


const char* MOD_file_name = NULL;
int* rule_line;			// <- new

struct conflict {
	const char* symbol;
	const char* type;
	int first_rule;
	int second_rule;
	int count;
};

struct conflict* conflicts = NULL;

int conflicts_capacity = 0;
int conflicts_count = 0;

void MOD_set_file_name(const char* filename)
{
	MOD_file_name = input_file_name; //! MODIFED
	for (int i = strlen(filename) - 1; i > 0; --i)
	{
		if (filename[i] == '/' || filename[i] == '\\')
		{
			MOD_file_name = filename + i + 1;
			return;
		}
	}

	MOD_file_name = filename;
}

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



char first_open_conflict_file = 0;
int conflict_count = 0;

char *c_output_file_name;
char *h_output_file_name;

void MOD_write_shifts(action* pref)
{
	shifts* sp = shift_table[pref->state];

	if (!sp)
		return;

	FILE* ffile = NULL;
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
		return;
	}
	for (int i = sp->nshifts - 1; i >= 0; i--) {
		int state = sp->shift[i];
		int symbol = accessing_symbol[state];
		if (i > 0)
			 fprintf(ffile, "%s -> ", symbol_name[symbol]);
		else
			 fprintf(ffile, "%s\n", symbol_name[symbol]);
	}
	fclose(ffile);

}

void MOD_write_conflicts(const action * firstRule, action * secondRule) {
	const char* symbol = symbol_name[firstRule->symbol];
	const char* type = firstRule->action_code == SHIFT ? "shift/reduce" : "reduce/reduce";

	if (conflicts_count == conflicts_capacity) {
		int new_capacity = (conflicts_capacity == 0) ? 1 : conflicts_capacity * 2;
		struct conflict* temp = realloc(conflicts, new_capacity * sizeof(struct conflict));
		if (temp == NULL) {
			free(conflicts);
		}
		conflicts = temp;
		conflicts_capacity = new_capacity;
	}
	for (int i = 0; i < conflicts_count; i++) {
		if (conflicts[i].type == type && conflicts[i].symbol == symbol && 
			conflicts[i].first_rule == rule_line[firstRule->number] && 
			conflicts[i].second_rule == rule_line[secondRule->number]) {
			conflicts[i].count++;
			return;
		}
	}
	conflicts[conflicts_count].type = type;
	conflicts[conflicts_count].symbol = symbol;
	conflicts[conflicts_count].count = 1;
	conflicts[conflicts_count].first_rule = rule_line[firstRule->number];
	conflicts[conflicts_count++].second_rule = rule_line[secondRule->number];
}

void MOD_print_errors() {
	FILE* ffile = NULL;
	if (!first_open_conflict_file) {
		ffile = fopen("conflicts_list.txt", "w");
		first_open_conflict_file = 1;
	}
	else {
		ffile = fopen("conflicts_list.txt", "a");
	}
	if (ffile == NULL) {
		error(lineno, 0, 0, "Cannot open conflicts_list file for writing %s", "conflicts_list.txt");
		return;
	}
	for (int i = 0; i < conflicts_count; i++) {
		fprintf(ffile, "%s: %s conflict on lines %s:%d VS %s:%d (%d times)\n", conflicts[i].symbol, conflicts[i].type,
			MOD_file_name, conflicts[i].first_rule,
			MOD_file_name, conflicts[i].second_rule,
			conflicts[i].count);
	}
	free(conflicts);
	fclose(ffile);
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

void MOD_reset_CheckReturn(CheckingReturn* data) {
	data->check_len = 0;
	data->check_return = 1;
}

void MOD_check_return(CheckingReturn *data, int c) {
	/*
		Checking that there is no "return" operator in the text of the rule.
		If a word starts with 'r', the next 7 characters are written to a special array,
		if the array is equal to the string "return ",
		that is, the "return" operator is found in the code,
		the program will warn of an error.
		*/
	if (data->check_return)
	{
		if (data->check_len < 6) data->check_line[data->check_len++] = c;
		if (data->check_len == 6)
		{
			data->check_line[6] = cptr[1];
			data->check_line[7] = '\0';
			if (strcmp(data->return_example, data->check_line) == 0)
			{
				return_err();
			}
			data->check_len++;
			data->check_return = 0;
		}
	}
}

void MOD_set_rule_line(int maxrules) {
	rule_line = NEW2(maxrules, int);
	if (rule_line == 0) no_space();
	rule_line[0] = 0;
	rule_line[1] = 0;
	rule_line[2] = 0;
}
