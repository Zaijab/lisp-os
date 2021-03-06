#include <stdio.h>
#include <stdlib.h>

#include "flisp.h"

#define __FLISP_VERSION__ "0.3"

void print_heap(size_t topbytes);

void rebuffer();

void save_image (char *fname);
void load_image (char *fname);
void load_file (char *fname);

FILE *readfile;

#define HEAP_SIZE (1024*1024)
#define SYMTAB_SIZE (1024*1024)

/*
environment toplevel;
*/

int main (int argc, char **argv) {
	void *heap, *expr;
	char *symt;
	type_error *err;

	printf("Welcome to flisp " __FLISP_VERSION__ " Copyright Frank James " __DATE__ "\n");

	/* create the heap */
	heap = calloc (HEAP_SIZE, sizeof(char));
	gc_init(heap, HEAP_SIZE);

	readfile = stdin;
	reader_bufferp = "";

	symt = calloc (SYMTAB_SIZE, sizeof(char));
	symbol_init (symt, SYMTAB_SIZE);

    env_init(&toplevel);

	/* set the refresh buffer function */
	refresh_buffer_f = &rebuffer;

	printf ("Loading flisp core ...\n");
	load_core ();

	/* launch the repl */
	flisp_repl (TRUE);

	/* clean up */
	free(heap);
	free (symt);

    return 0;
}


void print_heap (size_t topbytes) {
	void *p, *q;
	char c;
	printf ("heap: %d of %d (%d%%)\n",
			(int)(free_p - gc_working),
			gc_heap_size,
			(100*(int)(free_p - gc_working) / gc_heap_size));

	for (q = max(free_p - topbytes, gc_working); q < free_p; q += 16) {
		printf ("%08x  ", (unsigned int)q);

		for (p = q; (p < q + 8); p++) {
			if (p < free_p) {
				printf ("%02x ", (unsigned char)(*(unsigned char *)p));
			} else {
				printf ("   ");
			}
		}
		printf (" ");
		for (p = q + 8; (p < q + 16); p++) {
			if (p < free_p) {
				printf ("%02x ", (unsigned char)(*(unsigned char *)p));
			} else {
				printf ("   ");
			}
		}
		printf (" |");
		for (p = q; (p < q + 16); p++) {
			if (p < free_p) {
				c = (char)(*(char *)p);
				if (c >= ' ' && c <= '~') {
					printf ("%c", c);
				} else {
					printf (".");
				}
			} else {
				break;
			}
		}
		printf ("|\n");
	}
	printf ("%08x\n", (unsigned int)free_p);
}

void rebuffer() {
   	if (feof(readfile) != 0) {
		/* this is such a hack */
		strcpy(reader_buffer, "NIL");
	} else {
		fgets(reader_buffer, MAX_LINE, readfile);
	}

	reader_bufferp = reader_buffer;
}

/*
 * write all information to a file
 * first write the symbol table and then the contents of the heap
 */
void save_image (char *fname) {
	FILE *f;
	size_t size, i;
	char *p;

	f = fopen(fname, "wb");
	if (f != NULL) {
		if (fseek(f, 0, SEEK_SET) == 0) {

			/* write the number of strings */
			i = symbol_table - symbol_p;
			fwrite (&i, sizeof(size_t), 1, f);

			/* first write the strings */
			fwrite ((void *)string_table, sizeof(char), (string_table_p - string_table), f);

			/* now print the heap */
			fwrite ((void *)gc_working, sizeof(char), gc_heap_size / 2, f);
		}

		fclose(f);
	}
}

void save_image_obj (FILE *f, void *val) {
	gc_tag tag;
	tag = *CAST(gc_tag *, val);
	if (tag.forw == NULL) {
		switch (tag.type) {
		case TYPE_INT:
			fwrite (val, sizeof(type_int), 1, f);
			break;
		case TYPE_DOUBLE:
			fwrite (val, sizeof(type_double), 1, f);
			break;
		case TYPE_STRING:
			fwrite (val, sizeof(type_string), 1, f);
			break;
		case TYPE_SYMBOL:
			fwrite (val, sizeof(type_symbol), 1, f);
			break;
		case TYPE_CELL:
			fwrite (val, sizeof(type_cell), 1, f);
			break;
		case TYPE_HT:
			fwrite (val, sizeof(type_ht), 1, f);
			break;
		case TYPE_ARRAY:
			fwrite (val, sizeof(type_array), 1, f);
			break;
		case TYPE_PROC:
			fwrite (val, sizeof(type_proc), 1, f);
			break;
		case TYPE_CLOSURE:
			fwrite (val, sizeof(type_closure), 1, f);
			break;
		}
	}
}


void load_image (char *fname) {
	size_t n, i;
	FILE *f;
	char buffer[MAX_LINE];

	f = fopen (fname, "rb");
	if (f != NULL) {

		/* first intern the symbols. get the number of strings and then read them */
		fread (&n, sizeof(size_t), 1, f);
		for(i = 0; i < n; i++) {
			fscanf(f, "%s", buffer);
			intern(buffer);
		}

		/* now load the heap */

		fclose(f);
	}
}

/* evaluate contents of a file */
void load_file(char *fname) {
	FILE *f, *tmp;
	void *expr;

	f = fopen(fname, "r");
	if (f != NULL) {
		tmp = readfile;
		readfile = f;

		do {
			expr = next_expr();
			if (feof(readfile) != 0) {
				/* must've hit the end of the file */
				eval(expr, &toplevel);
				break;
			} else {
				eval(expr, &toplevel);
			}
		} while (expr != NULL);

		fclose(readfile);
		readfile = tmp;
	}
}

/* need these for internal printing operations */
void putch (unsigned char c) {
	putchar (c);
}

char getch () {
	return getchar ();
}
