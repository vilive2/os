#include "defs.h"
#define PARSEFUNC(name)                                                        \
	parse_##name(const struct parser_table *entry, char *argv[],           \
		     int *arg_ptr)

static bool PARSEFUNC(and);
static bool PARSEFUNC(or);
static bool PARSEFUNC(empty);
static bool PARSEFUNC(iname);
static bool PARSEFUNC(name);
static bool PARSEFUNC(negate);
static bool PARSEFUNC(type);
static bool PARSEFUNC(help);

#define PASTE(x, y) x##y
#define PARSE_OPTION(what, suffix)                                             \
	{ (ARG_OPTION), (what), PASTE(parse_, suffix), NULL }

#define PARSE_TEST(what, suffix)                                               \
	{ (ARG_TEST), (what), PASTE(parse_, suffix), PASTE(pred_, suffix) }

#define PARSE_ACTION(what, suffix)                                             \
	{ (ARG_ACTION), (what), PASTE(parse_, suffix), PASTE(pred_, suffix) }

#define PARSE_PUNCTUATION(what, suffix)                                        \
	{                                                                      \
		(ARG_PUNCTUATION), (what), PASTE(parse_, suffix),              \
		    PASTE(pred_, suffix)                                       \
	}

static struct parser_table const parse_table[] = {
    PARSE_PUNCTUATION("!", negate),
    PARSE_PUNCTUATION("(", openparen),
    PARSE_PUNCTUATION(")", closeparen),
    PARSE_PUNCTUATION("a", and),
    PARSE_PUNCTUATION("and", and),
    PARSE_TEST("empty", empty),
    PARSE_TEST("iname", iname),
    PARSE_TEST("name", name),
    PARSE_PUNCTUATION("o", or),
    PARSE_ACTION("print", print),
    PARSE_TEST("type", type),
    {ARG_TEST, "help", parse_help, NULL},
    {0, NULL, NULL, NULL}};

const struct parser_table *find_parser(const char *search_name) {
	int i;

	if (*search_name == '-')
		search_name++;

	for (i = 0; parse_table[i].parser_name != NULL; i++) {
		if (strcmp(parse_table[i].parser_name, search_name) == 0) {
			return &parse_table[i];
		}
	}

	return NULL;
}

static bool PARSEFUNC(and) {
	struct predicate *our_pred;

	our_pred = get_new_pred(entry);
	our_pred->pred_func = pred_and;
	our_pred->p_type = BI_OP;
	our_pred->p_prec = AND_PREC;
	our_pred->need_stat = our_pred->need_type = false;
	return true;
}

bool PARSEFUNC(closeparen) {
	struct predicate *our_pred;

	our_pred = get_new_pred(entry);
	our_pred->pred_func = pred_closeparen;
	our_pred->p_type = CLOSE_PAREN;
	our_pred->p_prec = NO_PREC;
	our_pred->need_stat = our_pred->need_type = false;
	return true;
}

static bool PARSEFUNC(empty) {
	struct predicate *our_pred;

	our_pred = insert_primary_withpred(entry, entry->pred_func, NULL);
	(void)our_pred;
	return true;
}

static bool collect_arg(char **argv, int *arg_ptr, const char **collected_arg) {
	if (argv == NULL || argv[*arg_ptr] == NULL) {
		*collected_arg = NULL;
		return false;
	} else {
		*collected_arg = argv[*arg_ptr];
		(*arg_ptr)++;
		return true;
	}
}

static bool PARSEFUNC(name) {
	const char *name;
	if (collect_arg(argv, arg_ptr, &name)) {
		struct predicate *our_pred;

		our_pred =
		    insert_primary_withpred(entry, entry->pred_func, name);
		our_pred->need_stat = our_pred->need_type = false;
		our_pred->args.str = name;
		return true;
	}
	return false;
}

static bool PARSEFUNC(iname) {
	const char *name;
	if (collect_arg(argv, arg_ptr, &name)) {
		struct predicate *our_pred;

		our_pred =
		    insert_primary_withpred(entry, entry->pred_func, name);
		our_pred->need_stat = our_pred->need_type = false;
		our_pred->args.str = name;
		return true;
	}

	return false;
}

static bool PARSEFUNC(negate) {
	struct predicate *our_pred;

	our_pred = get_new_pred_chk_op(entry, NULL);
	our_pred->pred_func = pred_negate;
	our_pred->p_type = UNI_OP;
	our_pred->p_prec = NEGATE_PREC;
	our_pred->need_stat = our_pred->need_type = false;
	return true;
}

bool PARSEFUNC(openparen) {
	struct predicate *our_pred;

	our_pred = get_new_pred_chk_op(entry, NULL);
	our_pred->pred_func = pred_openparen;
	our_pred->p_type = OPEN_PAREN;
	our_pred->p_prec = NO_PREC;
	our_pred->need_stat = our_pred->need_type = false;
	return true;
}

static bool PARSEFUNC(or) {
	struct predicate *our_pred;

	our_pred = get_new_pred(entry);
	our_pred->pred_func = pred_or;
	our_pred->p_type = BI_OP;
	our_pred->p_prec = OR_PREC;
	our_pred->need_stat = our_pred->need_type = false;
	return true;
}

bool PARSEFUNC(print) {
	struct predicate *our_pred;

	our_pred = insert_primary_withpred(entry, entry->pred_func, NULL);
	our_pred->need_stat = our_pred->need_type = false;
	return true;
}

bool PARSEFUNC(type) {
	struct predicate *our_pred;
	const char *typeletter;
	if (!collect_arg(argv, arg_ptr, &typeletter))
		return false;

	our_pred = insert_primary_withpred(entry, entry->pred_func, typeletter);
	our_pred->need_stat = false;
	our_pred->need_type = true;

	unsigned int type_cell;
	switch (*typeletter) {
		case 'b':
			type_cell = FTYPE_BLK;
			break;
		case 'c':
			type_cell = FTYPE_CHR;
			break;
		case 'd':
			type_cell = FTYPE_DIR;
			break;
		case 'f':
			type_cell = FTYPE_REG;
			break;
		case 'l':
			type_cell = FTYPE_LNK;
			break;
		case 'p':
			type_cell = FTYPE_FIFO;
			break;
		case 's':
			type_cell = FTYPE_SOCK;
			break;
		default:
			type_cell = 0;
			return false;
	}

	our_pred->args.types[type_cell] = true;

	return true;
}

void set_follow_state(enum SymlinkOption opt) {
	switch (opt) {
		case SYMLINK_ALWAYS_DEREF:
			options.xstat = optionl_stat;
			break;
		case SYMLINK_NEVER_DEREF:
			options.xstat = optionp_stat;
			break;
		default:
			options.xstat = optionp_stat;
	}

	options.symlink_handling = opt;
}

static bool PARSEFUNC(help) {
	(void)entry;
	(void)argv;
	(void)arg_ptr;

	cleanup();

	usage(EXIT_SUCCESS);
}