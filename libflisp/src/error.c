#include "error.h"

type_error *errors () {
	return error_p;
}

void error (char *message, char *location) {
	type_error *e = gc_malloc (sizeof(type_error));
	if (message != NULL) {
		e->message = gc_new_string(message);
	} else {
		e->message = gc_new_string("");
	}

	if (location != NULL) {
		e->location = gc_new_string(location);
	} else {
		e->location = gc_new_string("");
	}

	e->next = error_p;
	error_p = e;
}

void error_clear () {
	error_p = NULL;
}

void print_errors () {
  type_error *err;
  type_cell *errs;

  err = errors();
  if (err != NULL) {
    while (err != NULL) {
      errs = cons (err->message, cons (err->location, NULL));
      format_ ("Error: ~A at ~A~%", errs);
      err = err->next;
    }
  }
}
