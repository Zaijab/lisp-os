#ifndef GC_H
#define GC_H

#include "sys.h"
#include "types.h"
#include "symbol.h"
#include "procs.h"
#include "env.h"
#include "lists.h"

struct environment_;

/* globals used to keep track of the heap */
void *gc_working;
void *gc_free;
size_t gc_heap_size;
void *free_p;

/* initialisation function */
void gc_init (void *total_heap, size_t size);

/* memory allocation functions */
void *gc_malloc (size_t size);
type_int *gc_new_int (int i);
type_string *gc_new_string (char *str);
type_cell *gc_new_cell ();
type_symbol *gc_new_symbol (char *str);
type_double *gc_new_double (double d);
type_ht *gc_new_ht (size_t size);
type_array *gc_new_array (size_t size);
type_proc *gc_new_proc (flisp_proc_t proc);
type_closure *gc_new_closure (type_cell *params, type_cell *body, struct environment_ *env);

void *gc_new_copy (void *object);

/* functions to move objects around in the heap */
void gc_relocate_int (type_int **new, type_int *old);
void gc_relocate_string (type_string **new, type_string *old);
void gc_relocate_cell (type_cell **new, type_cell *old);
void gc_relocate_symbol (type_symbol **new, type_symbol *old);
void gc_relocate_double (type_double **new, type_double *old);
void gc_relocate_ht (type_ht **new, type_ht *old);
void gc_relocate_array (type_array **new, type_array *old);
void gc_relocate_proc (type_proc **new, type_proc *old);
void gc_relocate_closure (type_closure **new, type_closure *old);

void gc_relocate (void **new, void *old);

/* call gc_collect_init() and then gc_collect() on each of the root variables */
void gc_collect_init ();
void gc_collect (void **root);

#endif
