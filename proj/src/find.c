#include <fcntl.h>
#include <sys/types.h>
#include <fts.h>
#include "defs.h"

static bool find(char *arg);

static void visit(FTS *p, FTSENT *ent, struct stat *pstat) {
	struct predicate *eval_tree;

	state.have_stat =
	    (ent->fts_info != FTS_NS) && (ent->fts_info != FTS_NSOK);
	state.rel_pathname = ent->fts_accpath;
	// state.cwd_dir_fd = p->fts_cwd_fd;

	eval_tree = get_eval_tree();
	apply_predicate(ent->fts_path, pstat, eval_tree);
}

static void consider_visiting(FTS *p, FTSENT *ent) {
	struct stat statbuf;
	mode_t mode;
	int ignore;

	statbuf.st_ino = ent->fts_statp->st_ino;

	state.curdepth = ent->fts_level;

	if (ent->fts_info == FTS_ERR) {
		state.exit_status = EXIT_FAILURE;
		return;
	} else if (ent->fts_info == FTS_DNR) {
		state.exit_status = EXIT_FAILURE;
		if (options.do_dir_first)
			return;
	} else if (ent->fts_info == FTS_DC) {
		state.exit_status = EXIT_FAILURE;
		return;
	} else if (ent->fts_info == FTS_SLNONE) {
		return;
	} else if (ent->fts_info == FTS_NS) {
		return;
	}

	state.have_stat = true;
	state.have_type = true;
	statbuf = *(ent->fts_statp);
	state.type = mode = statbuf.st_mode;

	state.curdepth = ent->fts_level;

	ignore = 0;

	if (options.maxdepth >= 0) {
		if (ent->fts_level >= options.maxdepth) {
			fts_set(p, ent, FTS_SKIP);
			if (ent->fts_level > options.maxdepth)
				ignore = 1;
		}
	}

	if ((ent->fts_info == FTS_D) && !options.do_dir_first) {
		ignore = 1;
	} else if ((ent->fts_info == FTS_DP) && options.do_dir_first) {
		ignore = 1;
	} else if (ent->fts_level < options.mindepth) {
		ignore = 1;
	}

	if (!ignore) {
		visit(p, ent, &statbuf);
	}
}

static bool find(char *arg) {
	char *arglist[2];
	FTS *p;
	FTSENT *ent;

	state.starting_path_length = strlen(arg);

	arglist[0] = arg;
	arglist[1] = NULL;

	int ftsoptions = 0;

	switch (options.symlink_handling) {
		case SYMLINK_ALWAYS_DEREF:
			ftsoptions |= FTS_COMFOLLOW | FTS_LOGICAL;
			break;
		case SYMLINK_NEVER_DEREF:
			ftsoptions |= FTS_PHYSICAL;
			break;
		default:
			ftsoptions |= FTS_PHYSICAL;
			break;
	}

	if (options.stay_on_filesystem)
		ftsoptions |= FTS_XDEV;

	p = fts_open(arglist, ftsoptions, NULL);

	if (p == NULL) {
		error(0, errno, "cannot search %s", arg);
		state.exit_status = EXIT_FAILURE;
		return false;
	}

	while ((errno = 0, ent = fts_read(p)) != NULL) {
		state.have_stat = false;
		state.have_type = !!ent->fts_statp->st_mode;
		state.type = state.have_type ? ent->fts_statp->st_mode : 0;
		consider_visiting(p, ent);
	}

	if (errno) {
		error(
		    0, errno,
		    "failed to read file names from file system at or below %s",
		    arg);
		state.exit_status = EXIT_FAILURE;
		return false;
	}

	if (fts_close(p) != 0) {
		error(0, errno,
		      "failed to restore working directory after searching %s",
		      arg);
		state.exit_status = EXIT_FAILURE;
		return false;
	}

	p = NULL;

	return true;
}

bool process_all_startpoints(int argc, char *argv[]) {

	if (0 < argc && !looks_like_expression(argv[0], true)) {
		return find(argv[0]);
	} else {
		char defaultpath[2] = ".";
		return find(defaultpath);
	}
}
