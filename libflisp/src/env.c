#include "env.h"

void env_init(environment *env) {
	env->special = gc_new_ht(ENV_SIZE);
	env->lexical = NULL;

	env_define (env, intern("NIL"), NULL);
	env_define (env, intern("T"), intern("T"));

	env_define (env, intern("CAR"), gc_new_proc (&proc_car));
	env_define (env, intern("CDR"), gc_new_proc (&proc_cdr));
	env_define (env, intern("SET-CAR!"), gc_new_proc (&proc_set_car));
	env_define (env, intern("SET-CDR!"), gc_new_proc (&proc_set_cdr));
	env_define (env, intern("CONS"), gc_new_proc (&proc_cons));
	env_define (env, intern("LIST"), gc_new_proc (&proc_list));
	env_define (env, intern("REVERSE"), gc_new_proc (&proc_reverse));
	env_define (env, intern("LENGTH"), gc_new_proc (&proc_length));

	env_define (env, intern("+"), gc_new_proc (&proc_add));
	env_define (env, intern("-"), gc_new_proc (&proc_sub));
	env_define (env, intern("*"), gc_new_proc (&proc_mul));
	env_define (env, intern("/"), gc_new_proc (&proc_div));
	env_define (env, intern("MAKE-ARRAY"), gc_new_proc (&proc_make_array));
	env_define (env, intern("AREF"), gc_new_proc (&proc_aref));
	env_define (env, intern("SET-AREF!"), gc_new_proc (&proc_set_aref));

	env_define (env, intern("APPLY"), gc_new_proc (&proc_apply));
	env_define (env, intern("LOAD"), gc_new_proc (&proc_load));
	env_define (env, intern("EQ?"), gc_new_proc (&proc_eq));
	env_define (env, intern("EQL?"), gc_new_proc (&proc_eql));
	env_define (env, intern("EQUAL?"), gc_new_proc (&proc_equal));

	env_define (env, intern(">"), gc_new_proc (&proc_gt));
	env_define (env, intern("<"), gc_new_proc (&proc_lt));
	env_define (env, intern(">="), gc_new_proc (&proc_gte));
	env_define (env, intern("<="), gc_new_proc (&proc_lte));
	env_define (env, intern("="), gc_new_proc (&proc_e));

	env_define (env, intern ("MACROEXPAND"), gc_new_proc (&proc_macroexpand));
	env_define (env, intern ("GENSYM"), gc_new_proc (&proc_gensym));
	env_define (env, intern ("TYPE-OF"), gc_new_proc (&proc_typeof));

	env_define (env, intern ("PRINT"), gc_new_proc (&proc_print));
	env_define (env, intern ("PRINC"), gc_new_proc (&proc_princ));
	env_define (env, intern ("FORMAT"), gc_new_proc (&proc_format));
	env_define (env, intern ("TOPLEVEL"), gc_new_proc (&proc_toplevel));
	env_define (env, intern ("GETHASH"), gc_new_proc (&proc_gethash));
	env_define (env, intern ("SETHASH"), gc_new_proc (&proc_sethash));
	env_define (env, intern ("REMHASH"), gc_new_proc (&proc_remhash));
	env_define (env, intern ("HEAP"), gc_new_proc (&proc_heap));

    env_define (env, intern ("APPEND"), gc_new_proc (&proc_append));
    env_define (env, intern ("COPY-LIST"), gc_new_proc (&proc_copy_list));
	env_define (env, intern ("ERROR"), gc_new_proc (&proc_error));

	/* math related stuff */
	env_define (env, intern ("PI"), gc_new_double (PI));
	env_define (env, intern ("COS"), gc_new_proc (&proc_cos));
	env_define (env, intern ("SIN"), gc_new_proc (&proc_sin));
	env_define (env, intern ("TAN"), gc_new_proc (&proc_tan));
	env_define (env, intern ("ACOS"), gc_new_proc (&proc_acos));
	env_define (env, intern ("ASIN"), gc_new_proc (&proc_asin));
	env_define (env, intern ("ATAN"), gc_new_proc (&proc_atan));
	env_define (env, intern ("COSH"), gc_new_proc (&proc_cosh));
	env_define (env, intern ("SINH"), gc_new_proc (&proc_sinh));
	env_define (env, intern ("TANH"), gc_new_proc (&proc_tanh));
	env_define (env, intern ("ACOSH"), gc_new_proc (&proc_acosh));
	env_define (env, intern ("ASINH"), gc_new_proc (&proc_asinh));
	env_define (env, intern ("ATANH"), gc_new_proc (&proc_atanh));
	env_define (env, intern ("EXP"), gc_new_proc (&proc_exp));
	env_define (env, intern ("LOG"), gc_new_proc (&proc_log));
	env_define (env, intern ("LOG10"), gc_new_proc (&proc_log10));
	env_define (env, intern ("SQRT"), gc_new_proc (&proc_sqrt));
	env_define (env, intern ("CEILING"), gc_new_proc (&proc_ceil));
	env_define (env, intern ("ABS"), gc_new_proc (&proc_fabs));
	env_define (env, intern ("FLOOR"), gc_new_proc (&proc_floor));
	env_define (env, intern ("ERF"), gc_new_proc (&proc_erf));
	env_define (env, intern ("ERFC"), gc_new_proc (&proc_erfc));
	env_define (env, intern ("GAMMA"), gc_new_proc (&proc_gamma));
	env_define (env, intern ("BESSEL-J0"), gc_new_proc (&proc_j0));
	env_define (env, intern ("BESSEL-J1"), gc_new_proc (&proc_j1));
	env_define (env, intern ("BESSEL-J"), gc_new_proc (&proc_jn));
	env_define (env, intern ("LGAMMA"), gc_new_proc (&proc_lgamma));
	env_define (env, intern ("BESSEL-Y0"), gc_new_proc (&proc_y0));
	env_define (env, intern ("BESSEL-Y1"), gc_new_proc (&proc_y1));
	env_define (env, intern ("BESSEL-Y"), gc_new_proc (&proc_yn));

	env_define (env, intern ("ASSOC"), gc_new_proc (&proc_assoc));
}

/* lookup a binding in the environment. search through the lexical frames before trying the toplevel */
bool lookup(void **val, type_symbol *sym, environment *env) {
    bool found;
	void *tmp;
	type_cell *frame;


	frame = env->lexical;
	while (frame != NULL) {
		tmp = assoc(CAST(void *, sym), frame->car);
		if (tmp != NULL) {
			*val = CAST(type_cell *, tmp)->cdr;
			return TRUE;
		}
		frame = frame->cdr;
	}

	found = gethash(env->special, sym, val);
	return found;
}

environment *extend_env (environment *env, type_cell *syms, type_cell *vals) {
	type_cell *frame = NULL;
	while (syms != NULL) {
		if (get_type(syms) == TYPE_SYMBOL) {
			/* dotted list */
			frame = acons(syms, vals, frame);
			break;
		} else {
			frame = acons(cell_car(syms), cell_car(vals), frame);
		}

		syms = cell_cdr(syms);
		vals = cell_cdr(vals);
	}

	/* push the frame onto the lexical bindings */
	cell_push (&(env->lexical), frame);

	/*	env->lexical = cons(frame, env->lexical); */
	return env;
}

environment *rebind_env (environment *env, type_cell *vals) {
    type_cell *frame = cell_car (env->lexical);
    type_cell *c;

    while (frame != NULL) {
        c = cell_car (frame);
        c->cdr = cell_car (vals);
        vals = vals->cdr;
        frame = frame->cdr;
    }

    return env;
}

environment *remove_frame (environment *env) {
	cell_pop (&(env->lexical));
	return env;
}

environment *extend_env_special (environment *env, type_cell *syms, type_cell *vals) {
	while (syms != NULL && vals != NULL) {
		env_define(env, syms->car, vals->car);
		syms = syms->cdr;
		vals = vals->cdr;
	}

	return env;
}

void env_define (environment *env, type_symbol *sym, void *val) {
	sethash(&(env->special), sym, val);
}

void env_set (environment *env, type_symbol *sym, void *val) {
	/* lookup in lexical vars first, then try special */
	type_cell *frames, *frame, *binding;

	frames = env->lexical;
	while (frames != NULL) {
		frame = cell_car(frames);
		while (frame != NULL) {
			/* frame = ((sym . val) ...) c->car = (sym . val) */
			binding = cell_car(frame);
			if (eq(cell_car(binding), sym)) {
				binding->cdr = val;
				return;
			}
			frame = frame->cdr;
		}
		frames = frames->cdr;
	}

	/* not found in lexical frames, set at top level */
	sethash(&(env->special), sym, val);
}
