#ifndef TYPES_H
#define TYPES_H

#include "sys.h"

#define TYPE_INT                0
#define TYPE_STRING             1
#define TYPE_CELL               2
#define TYPE_SYMBOL             3
#define TYPE_DOUBLE             4
#define TYPE_HT                 5
#define TYPE_ARRAY              6
#define TYPE_NULL               7
#define TYPE_PROC               8
#define TYPE_CLOSURE            9
#define TYPE_STRUCT             10

#define CAST(type, x)           ((type)(x))

static char *type_names[] = {"INT", "STRING", "CONS", "SYMBOl", "DOUBLE", "HASH-TABLE", "ARRAY", "NULL", "PROC", "CLOSURE", "STRUCT"};


typedef unsigned int gc_type;

/* garbage collector tag that all types must contain */
typedef struct {
	gc_type type;
	void *forw;
} gc_tag;


/* the different types follow */

typedef struct {
	gc_tag tag;
	int i;
} type_int;

typedef struct {
	gc_tag tag;
	char *str;
	size_t len;
} type_string;

typedef struct {
	gc_tag tag;
	void *car;
	void *cdr;
} type_cell;

typedef struct {
	gc_tag tag;
	char *sym;
} type_symbol;

typedef struct {
	gc_tag tag;
	double d;
} type_double;

typedef struct {
	gc_tag tag;
	type_cell **buckets;
	size_t size;
	size_t fill;
} type_ht;

typedef struct {
	gc_tag tag;
	void **data;
	size_t size;
} type_array;


typedef void * (*flisp_proc_t)(type_cell *);

typedef struct {
  gc_tag tag;
  flisp_proc_t proc;
} type_proc;

struct environment_;

typedef struct {
  gc_tag tag;
  type_cell *params;
  type_cell *body;
  struct environment_ *env;
} type_closure;

/* user defined structures are just an array of object pointers */
typedef struct {
	gc_tag tag;
	size_t nslots;
	void **slots;
} type_struct;


gc_type get_type (void *val);

#endif
