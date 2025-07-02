#include <fcntl.h>
#include <assert.h>
#include "defs.h"

struct predicate *insert_primary_withpred(const struct parser_table *entry,
					  PRED_FUNC pred_func,
					  const char *arg) {
	struct predicate *new_pred;

	new_pred = get_new_pred_chk_op(entry, arg);
	new_pred->pred_func = pred_func;
	new_pred->p_name = entry->parser_name;
	new_pred->args.str = NULL;
	new_pred->p_type = PRIMARY_TYPE;
	new_pred->p_prec = NO_PREC;
	return new_pred;
}

void usage(int status) {
	if (status != EXIT_SUCCESS) {
		fprintf(stderr, "Try '%s -help' for more information.\n",
			program_name);
		exit(status);
	}

	fprintf(stdout, "Usage: %s [follow-link] [start-dir] [expression]\n",
		program_name);
	fputs("\nfollow link is optional, default do not follow sym lnk, -L "
	      "follow\n",
	      stdout);
	fputs("\nstart-dir is optional parameter.\n", stdout);
	fputs("\nSearch start from start-dir.\n", stdout);
	fputs("\nCurrent directory is default start-dir.\n", stdout);
	fputs("\nexpression is required parameter.\n", stdout);
	fputs("\nExpression:\n", stdout);
	fputs("\t-depth\n\t\tprocess directory content before the directory.\n",
	      stdout);
	fputs("\t-maxdepth level\n\t\tdescend at most level below the starting "
	      "dir.\n",
	      stdout);
	fputs("\t-mindepth level\n\t\tdo not process below level.\n", stdout);
	fputs("\t-executable\n\t\tmatches files which are executable and "
	      "directories which are searchable by the current user.\n",
	      stdout);
	fputs("\t-iname pattern\n\t\tlike -name, but the match is case "
	      "insensitive.\n",
	      stdout);
	fputs("\t-links n\n\t\tfile has less than, more than or exactcly n "
	      "hard links.\n",
	      stdout);
	fputs("\t\t-n : less than n\n\t\t+n : greater than n\n\t\tn : equal to "
	      "n\n",
	      stdout);
	fputs("\t-name pattern\n\t\tfilename shall match pattern.\n", stdout);
	fputs("\t-type c\n\t\tfile is of type c.\n", stdout);
	fputs("\t\tb\t\tblock special\n"
	      "\t\tc\t\tcharacter special\n"
	      "\t\td\t\tdirectory\n"
	      "\t\tp\t\tnamed pipe\n"
	      "\t\tf\t\tregular file\n"
	      "\t\tl\t\tsymbolic link\n"
	      "\t\ts\t\tsocket\n",
	      stdout);
	fputs("\t-size n[cwbkMG]\n\t\tfile uses less than, more than or "
	      "exactly n units of space, rounding up.\n",
	      stdout);
	fputs("\t\tb\t\t512 byte blocks (default)\n"
	      "\t\tc\t\tbytes\n"
	      "\t\tw\t\ttwo-byte words\n"
	      "\t\tk\t\tkibibytes (1024 bytes)\n"
	      "\t\tM\t\tmebibytes (1024*1024 bytes)\n"
	      "\t\tG\t\tgibibytes (1024*1024*1024 bytes)\n",
	      stdout);

	fputs("\t-user uname\n\t\tfile is owned by uname.\n", stdout);

	exit(status);
}

int get_statinfo(const char *pathname, const char *name, struct stat *p) {
	if (!state.have_stat) {
		(*options.xstat)(name, p);
	}

	state.have_stat = true;
	state.have_type = true;
	state.type = p->st_mode;

	return 0;
}

static int get_info(const char *pathname, struct stat *p,
		    struct predicate *pred_ptr) {
	bool todo = false;

	if (pred_ptr->need_stat && !state.have_stat) {
		todo = true;
	} else if (pred_ptr->need_type && !state.have_type) {
		todo = true;
	}

	if (todo) {
		if (get_statinfo(pathname, state.rel_pathname, p) != 0)
			return -1;
	}

	return 0;
}

int optionl_stat(const char *name, struct stat *p) {
	return fstatat(state.cwd_dir_fd, name, p, 0);
}

int optionp_stat(const char *name, struct stat *p) {
	assert((state.cwd_dir_fd >= 0) || (state.cwd_dir_fd == AT_FDCWD));
	return fstatat(state.cwd_dir_fd, name, p, AT_SYMLINK_NOFOLLOW);
}

bool looks_like_expression(const char *arg, bool leading) {
	switch (arg[0]) {
		case '-':
			if (arg[1])
				return true;
			else
				return false;
			break;

		case ')':
		case ',':
			if (arg[1])
				return false;
			else
				return !leading;

		case '!':
		case '(':
			if (arg[1])
				return false;
			else
				return true;

		default:
			return false;
	}
}

int process_leading_options(int argc, char *argv[]) {
	int i;

	for (i = 1; i < argc; i++) {
		if (strcmp("-L", argv[i]) == 0) {
			// dereference all symbolic links.
			set_follow_state(SYMLINK_ALWAYS_DEREF);
		} else {
			break;
		}
	}

	return i;
}

void set_option_defaults(struct options *p) {
	p->do_dir_first = true;
	p->explicit_depth = false;
	p->maxdepth = p->mindepth = -1;

	p->stay_on_filesystem = true;
	set_follow_state(SYMLINK_NEVER_DEREF);
}

bool apply_predicate(const char *pathname, struct stat *stat_buf,
		     struct predicate *p) {

	if (p->need_stat || p->need_type) {
		if (get_info(pathname, stat_buf, p) != 0)
			return false;
	}

	if ((p->pred_func)(pathname, stat_buf, p)) {
		return true;
	} else {
		return false;
	}
}

void cleanup() {
	free_predicates();
}
