#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>

#include "defs.h"

#define PREDFUNC(name)                                                         \
	pred_##name(const char *pathname, struct stat *stat_buf,               \
		    struct predicate *pred_ptr)

bool PREDFUNC(and) {
	if (pred_ptr->pred_left == NULL ||
	    apply_predicate(pathname, stat_buf, pred_ptr->pred_left)) {
		return apply_predicate(pathname, stat_buf,
				       pred_ptr->pred_right);
	} else {
		return false;
	}
}

bool PREDFUNC(comma) {
	if (pred_ptr->pred_left != NULL) {
		apply_predicate(pathname, stat_buf, pred_ptr->pred_left);
	}
	return apply_predicate(pathname, stat_buf, pred_ptr->pred_right);
}

bool PREDFUNC(empty) {
	if (S_ISDIR(stat_buf->st_mode)) {
		int fd;
		DIR *d;
		struct dirent *dp;
		bool empty = true;
		if ((fd = openat(state.cwd_dir_fd, state.rel_pathname,
				 O_RDONLY | O_CLOEXEC | O_DIRECTORY)) < 0) {
			return false;
		}

		d = fdopendir(fd);
		if (d == NULL) {
			close(fd);
			return false;
		}

		for (dp = readdir(d); dp; dp = readdir(d)) {
			if (strcmp(".", dp->d_name) == 0 ||
			    strcmp("..", dp->d_name) == 0)
				continue;
			else {
				empty = false;
				break;
			}
		}
		closedir(d);
		return empty;
	} else if (S_ISREG(stat_buf->st_mode)) {
		return stat_buf->st_size == 0;
	} else {
		return false;
	}
}

static bool pred_name_common(const char *pathname, const char *str, int flags) {
	const char *filename = pathname;
	const char *ptr = pathname;
	while (*ptr) {
		if (*ptr == '/')
			filename = ptr + 1;
		ptr++;
	}

	return match(filename, str, flags);
}

bool PREDFUNC(iname) {
	return pred_name_common(pathname, pred_ptr->args.str, IGNORE_CASE);
}

bool PREDFUNC(name) {
	return pred_name_common(pathname, pred_ptr->args.str, 0);
}

bool PREDFUNC(negate) {
	return !apply_predicate(pathname, stat_buf, pred_ptr->pred_right);
}

bool PREDFUNC(or) {
	if (pred_ptr->pred_left == NULL ||
	    !apply_predicate(pathname, stat_buf, pred_ptr->pred_left)) {
		return apply_predicate(pathname, stat_buf,
				       pred_ptr->pred_right);
	} else {
		return true;
	}
}

bool PREDFUNC(print) {
	mode_t mode;
	if (state.have_stat)
		mode = stat_buf->st_mode;
	else
		mode = state.type;
	char *file_type;

	if (S_ISREG(mode))
		file_type = "REG";
	else if (S_ISDIR(mode))
		file_type = "DIR";
	else if (S_ISLNK(mode))
		file_type = "LNK";
	else if (S_ISBLK(mode))
		file_type = "BLK";
	else if (S_ISCHR(mode))
		file_type = "CHR";
	else if (S_ISSOCK(mode))
		file_type = "SCK";
	else if (S_ISFIFO(mode))
		file_type = "FIF";
	else
		file_type = "UNK";

	printf("%-5s%s\n", file_type, pathname);
	return true;
}

bool PREDFUNC(type) {
	mode_t mode;
	enum file_type type = FTYPE_COUNT;

	if (state.have_stat)
		mode = stat_buf->st_mode;
	else
		mode = state.type;

	if (S_ISREG(mode))
		type = FTYPE_REG;
	else if (S_ISDIR(mode))
		type = FTYPE_DIR;
	else if (S_ISLNK(mode))
		type = FTYPE_LNK;
	else if (S_ISBLK(mode))
		type = FTYPE_BLK;
	else if (S_ISCHR(mode))
		type = FTYPE_CHR;
	else if (S_ISSOCK(mode))
		type = FTYPE_SOCK;
	else if (S_ISFIFO(mode))
		type = FTYPE_FIFO;

	if ((type != FTYPE_COUNT) && pred_ptr->args.types[type])
		return true;
	else
		return false;
}

bool PREDFUNC(closeparen) {
	return true;
}

bool PREDFUNC(openparen) {
	return true;
}