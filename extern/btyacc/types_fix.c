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


// read_types() a function that reads the entire grammar file and search all types declarations

void read_types(){
	FILE* input_file;
	int c;
	int i;
	char saw_eoff = 0;
	input_file = fopen(input_file_name, "r");
	types = 1;
NextLine:;
	i = 0;
	// checking the end of the file
	if (saw_eoff || (c = getc(input_file)) == EOF) {
		if (line) FREE(line);
		line = NULL;
		saw_eoff = 1;
		lineno = 0;
		cptr = NULL;
		types = 0;
		return;
	}

	// managing memory allocation for the line buffer
	if (line == 0 || linesize != (LINESIZE + 1)) {
		if (line) FREE(line);
		linesize = LINESIZE + 1;
		if (!(line = MALLOC(linesize))) no_space();
	}
	++lineno;

	// reading a line
	while ((line[i] = c) != '\n') {
		if (++i + 1 >= linesize)
			if (!(line = REALLOC(line, linesize += LINESIZE)))
				no_space();
		if ((c = getc(input_file)) == EOF) {
			c = '\n';
			saw_eoff = 1;
		}
	}
	line[i + 1] = 0;
	cptr = line;
	// check if line starts with "%type "
	if (strncmp(&line[0], "%type ", 6) == 0) {
		// skip substring "%type "
		cptr += 6;
		// write new type to the program
		declare_types();
	}
	goto NextLine;
}