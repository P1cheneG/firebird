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