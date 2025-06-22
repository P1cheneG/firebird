#include "defs.h"

static void declare_argtypes_types(bucket* bp)
{
	char* tags[MAXARGS];
	int	args = 0, c;

	if (bp->args >= 0) retyped_warning(bp->name);
	cptr++; /* skip open paren */
	for (;;) {
		c = nextc_line();
		if (c == '\n') unexpected_endline();
		if (c != '<') syntax_error(lineno, line, cptr);
		tags[args++] = get_tag_types();
		c = nextc_line();
		if (c == ')') break;
		if (c == '\n') unexpected_endline();
	}
	cptr++; /* skip close paren */
	bp->args = args;
	if (!(bp->argnames = NEW2(args, char*))) no_space();
	if (!(bp->argtags = NEW2(args, char*))) no_space();
	while (--args >= 0) {
		bp->argtags[args] = tags[args];
		bp->argnames[args] = 0;
	}
}

void declare_types_types()
{
	register int c;
	register bucket* bp = 0;
	char* tag = 0;

	c = nextc_line();
	if (c == '<') {
		tag = get_tag_types();
		c = nextc_line();
	}
	if (c == '\n') unexpected_endline();

	for (;;) {
		c = nextc_line();
		if (isalpha(c) || c == '_' || c == '.' || c == '$') {
			bp = get_name_types();
			if (nextc_line() == '(')
				declare_argtypes_types(bp);
			else
				bp->args = 0;
		}
		else if (c == '\'' || c == '"') {
			bp = get_literal();
			bp->args = 0;
		}
		else
			return;

		if (tag) {
			if (bp->tag && tag != bp->tag)
				retyped_warning(bp->name);
			bp->tag = tag;
		}
	}
}


char* get_tag_types()
{
	register int c;
	int t_lineno = lineno;
	char* t_line = dup_line();
	char* t_cptr = t_line + (cptr - line);

	++cptr;
	c = nextc_line();
	if (c == '\n') unexpected_endline();
	if (!isalpha(c) && c != '_' && c != '$')
		illegal_tag(t_lineno, t_line, t_cptr);

	cinc = 0;
	do { cachec(c); c = *++cptr; } while (IS_IDENT(c));

	c = nextc_line();
	if (c == '\n') unexpected_endline();
	if (c != '>')
		illegal_tag(t_lineno, t_line, t_cptr);
	++cptr;

	FREE(t_line);
	havetags = 1;
	return cache_tag(cache, cinc);
}


bucket* get_name_types()
{
	register int c;

	cinc = 0;
	for (c = *cptr; IS_IDENT(c); c = *++cptr)
		cachec(c);
	cachec(NUL);
	if (is_reserved(cache)) used_reserved(cache);
	return (lookup(cache));
}





int nextc_line()
{
	register char* s;

	if (line == 0)
		return (EOF);
	s = cptr;
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




void get_types() {
	FILE* ff;
	int c;
	int i;
	char saw_eoff = 0;
	ff = fopen(input_file_name, "r");
	cptr = NULL;
	types = 1;
NextLine:;
	i = 0;

	if (saw_eoff || (c = getc(ff)) == EOF) {
		if (line) FREE(line);
		line = NULL;
		saw_eoff = 1;
		lineno = 0;
		cptr = NULL;
		types = 0;
		return;
	}
	if (line == 0 || linesize != (LINESIZE + 1)) {
		if (line) FREE(line);
		linesize = LINESIZE + 1;
		if (!(line = MALLOC(linesize))) no_space();
	}
	++lineno;
	while ((line[i] = c) != '\n') {
		if (++i + 1 >= linesize)
			if (!(line = REALLOC(line, linesize += LINESIZE)))
				no_space();
		if ((c = getc(ff)) == EOF) {
			c = '\n';
			saw_eoff = 1;
		}
	}
	line[i + 1] = 0;
	cptr = line;
	if (strncmp(&line[0], "%type ", 6) == 0) {
		cptr += 6;
		declare_types_types();
	}
	goto NextLine;
}