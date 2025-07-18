#include "defs.h"

// next_char() a function that skips spaces and delimiters in a string for read_types() function

const char* MOD_file_name = NULL;
int* rule_line;			// <- new



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
	FILE * ffile = NULL;
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
	const char* symbol = symbol_name[firstRule->symbol];
	const char* type = firstRule->action_code == SHIFT ? "shift/reduce" : "reduce/reduce";
	//const char* bp = symbol_name[firstRuleno];
	fprintf(ffile, "%s: %s conflict on lines %s:%d VS %s:%d\n", symbol, type,
	MOD_file_name, rule_line[firstRule->number],
	MOD_file_name, rule_line[secondRule->number]);
	if (firstRule->action_code == SHIFT) {
		MOD_write_shifts(firstRule);
		MOD_write_shifts(secondRule);
	}
	fclose(ffile);
}