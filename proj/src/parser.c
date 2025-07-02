#include <inttypes.h>
#include <pwd.h>
#include "defs.h"
#define PARSEFUNC(name)                                                        \
	parse_##name(const struct parser_table *entry, char *argv[],           \
		     int *arg_ptr)

static bool PARSEFUNC(and);
static bool PARSEFUNC(depth);
static bool PARSEFUNC(empty);
static bool PARSEFUNC(executable);
static bool PARSEFUNC(help);
static bool PARSEFUNC(iname);
static bool PARSEFUNC(links);
static bool PARSEFUNC(maxdepth);
static bool PARSEFUNC(mindepth);
static bool PARSEFUNC(name);
static bool PARSEFUNC(negate);
static bool PARSEFUNC(or);
static bool PARSEFUNC(size);
static bool PARSEFUNC(type);
static bool PARSEFUNC(user);

static bool get_num(const char *str, uintmax_t *num,
		    enum comparison_type *comp_type);
static struct predicate *insert_num(char *argv[], int *arg_ptr,
				    const struct parser_table *entry);

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
    PARSE_OPTION("depth", depth),
    PARSE_TEST("empty", empty),
    PARSE_TEST("executable", executable),
    PARSE_TEST("iname", iname),
    PARSE_TEST("links", links),
    PARSE_OPTION("maxdepth", maxdepth),
    PARSE_OPTION("mindepth", mindepth),
    PARSE_TEST("name", name),
    PARSE_PUNCTUATION("o", or),
    PARSE_ACTION("print", print),
    PARSE_TEST("size", size),
    PARSE_TEST("type", type),
    PARSE_TEST("user", user),
    {ARG_TEST, "help", parse_help, NULL},
    {ARG_TEST, "h", parse_help, NULL},
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

static bool PARSEFUNC(depth) {
	(void)entry;
	(void)argv;
	options.do_dir_first = false;
	return true;
}

static bool PARSEFUNC(empty) {
	struct predicate *our_pred;

	our_pred = insert_primary_withpred(entry, entry->pred_func, NULL);
	(void)our_pred;
	return true;
}

static bool PARSEFUNC(executable) {
	struct predicate *our_pred;
	our_pred = insert_primary_withpred(entry, entry->pred_func, NULL);
	our_pred->need_stat = our_pred->need_type = false;
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

static bool insert_depthspec(const struct parser_table *entry, char **argv,
			     int *arg_ptr, int *limitptr) {
	const char *depthstr;
	int depth_len;
	const char *predicate = argv[(*arg_ptr) - 1];
	if (collect_arg(argv, arg_ptr, &depthstr)) {
		depth_len = strspn(depthstr, "0123456789");
		if ((depth_len > 0) && (depthstr[depth_len] == 0)) {
			*limitptr = atoi(depthstr);
			return true;
		}
		cleanup();
		error(EXIT_FAILURE, 0,
		      "Expected a positive decimal integer argument to %s, but "
		      "got %s",
		      predicate, depthstr);
	}

	return false;
}

static bool PARSEFUNC(maxdepth) {
	return insert_depthspec(entry, argv, arg_ptr, &options.maxdepth);
}

static bool PARSEFUNC(mindepth) {
	return insert_depthspec(entry, argv, arg_ptr, &options.mindepth);
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

static bool PARSEFUNC(links) {
	struct predicate *p = insert_num(argv, arg_ptr, entry);
	return p != NULL;
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

static bool PARSEFUNC(size) {
	struct predicate *our_pred;
	char *arg;
	uintmax_t num;
	char suffix;
	enum comparison_type c_type;
	int blksize = 512;
	int len;

	if (argv == NULL || argv[*arg_ptr] == NULL)
		return false;
	arg = argv[*arg_ptr];
	len = strlen(arg);
	if (len == 0) {
		cleanup();
		error(EXIT_FAILURE, 0, "invalid null argument to -size");
	}

	suffix = arg[len - 1];

	switch (suffix) {
		case 'b':
			blksize = 512;
			arg[len - 1] = '\0';
			break;
		case 'c':
			blksize = 1;
			arg[len - 1] = '\0';
			break;
		case 'k':
			blksize = 1024;
			arg[len - 1] = '\0';
			break;
		case 'M':
			blksize = 1024 * 1024;
			arg[len - 1] = '\0';
			break;
		case 'G':
			blksize = 1024 * 1024 * 1025;
			arg[len - 1] = '\0';
			break;
		case 'w':
			blksize = 2;
			arg[len - 1] = '\0';
			break;

		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			suffix = 0;
			break;
		default:
			cleanup();
			error(EXIT_FAILURE, 0, "invalid -size type `%c'",
			      suffix);
	}

	if (!get_num(arg, &num, &c_type)) {
		cleanup();
		error(EXIT_FAILURE, 0, "Invalid argument `%s%c' to -size", arg,
		      suffix);
	}

	our_pred = insert_primary_withpred(entry, entry->pred_func, arg);
	our_pred->args.size.kind = c_type;
	our_pred->args.size.blocksize = blksize;
	our_pred->args.size.size = num;
	our_pred->need_stat = true;
	our_pred->need_type = false;

	(*arg_ptr)++;
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

static bool PARSEFUNC(user) {
	const char *username;
	if (collect_arg(argv, arg_ptr, &username)) {
		struct predicate *our_pred;
		uid_t uid;
		struct passwd *cur_pwd = getpwnam(username);
		endpwent();
		if (cur_pwd != NULL) {
			uid = cur_pwd->pw_uid;
		} else {
			uintmax_t num;
			char *pend;
			num = strtoumax(username, &pend, 10);
			if (*pend) {
				cleanup();
				error(EXIT_FAILURE, 0,
				      "invalid user name or UID argument to "
				      "-user: %s",
				      username);
			}
			uid = num;
		}

		our_pred =
		    insert_primary_withpred(entry, entry->pred_func, username);
		our_pred->args.uid = uid;
		return true;
	}

	return false;
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

static struct predicate *insert_num(char **argv, int *arg_ptr,
				    const struct parser_table *entry) {
	const char *numstr;
	if (collect_arg(argv, arg_ptr, &numstr)) {
		uintmax_t num;
		enum comparison_type c_type;
		if (get_num(numstr, &num, &c_type)) {
			struct predicate *our_pred = insert_primary_withpred(
			    entry, entry->pred_func, numstr);
			our_pred->args.numinfo.kind = c_type;
			our_pred->args.numinfo.l_val = num;
			return our_pred;
		} else {
			const char *predicate = argv[(*arg_ptr) - 2];
			cleanup();
			error(EXIT_FAILURE, 0, "non-numeric argument to %s: %s",
			      predicate, numstr);
		}
	}

	return NULL;
}

static void get_comp_type(const char **str, enum comparison_type *comp_type) {
	switch (**str) {
		case '+':
			*comp_type = COMP_GT;
			(*str)++;
			break;
		case '-':
			*comp_type = COMP_LT;
			(*str)++;
			break;
		default:
			*comp_type = COMP_EQ;
			break;
	}
}

static bool get_num(const char *str, uintmax_t *num,
		    enum comparison_type *comp_type) {
	char *pend;
	if (str == NULL)
		return false;

	if (comp_type) {
		get_comp_type(&str, comp_type);
	}

	*num = strtoumax(str, &pend, 10);
	if (*pend)
		return false;
	return true;
}