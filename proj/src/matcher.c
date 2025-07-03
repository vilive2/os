#include <ctype.h>

#include "defs.h"

void To_Lowercase(char *string) {
	while (*string) {
		if (isupper(*string) != 0) {
			*string = tolower(*string);
		}
		string++;
	}
}

int match(const char *name, const char *pattern, int flags) {
	if (flags & IGNORE_CASE) {
		char *new_name = (char *)malloc(strlen(name) + 1);
		if (new_name == NULL) {
			cleanup();
			error(EXIT_FAILURE, 0, "malloc failed");
			return 0;
		}
		char *new_pattern = (char *)malloc(strlen(pattern) + 1);
		if (new_pattern == NULL) {
			free(new_name);
			cleanup();
			error(EXIT_FAILURE, 0, "malloc failed");
			return 0;
		}
		strcpy(new_name, name);
		strcpy(new_pattern, pattern);
		To_Lowercase(new_name);
		To_Lowercase(new_pattern);
		int res = match(new_name, new_pattern, 0);
		free(new_name);
		free(new_pattern);
		return res;
	}

	if (*pattern == '\0') {
		if (*name == '\0') {
			return 1;
		} else {
			return 0;
		}
	}

	if (*pattern == '*') {
		while (*pattern == '*') {
			pattern++;
		}
		if (*pattern == '\0') {
			return 1;
		}
		while (*name) {
			if (match(name, pattern, flags)) {
				return 1;
			}
			name++;
		}
		return 0;
	}

	if (*pattern == '?') {
		if (*name == '\0') {
			return 0;
		}
		return match(name + 1, pattern + 1, flags);
	}

	if (*pattern == '[') {
		pattern++;
		int match_found = 0;
		int negate = 0;

		if (*pattern == '^') {
			negate = 1;
			pattern++;
		}

		while (*pattern != ']' && *pattern != '\0') {
			// if (*pattern == *name)
			// {
			//     match_found =1;
			// }
			// pattern++;
			if (*(pattern + 1) == '-' && *(pattern + 2) != '\0' &&
			    *(pattern + 2) != ']') {
				char start = *pattern;
				char end = *(pattern + 2);
				if (*name >= start && *name <= end) {
					match_found = 1;
				}
				pattern = pattern + 3;
			} else {
				if (*pattern == *name) {
					match_found = 1;
				}
				pattern++;
			}
		}
		if (*pattern != ']') {
			return 0;
		}

		if ((match_found && !negate) || (!match_found && negate)) {
			return match(name + 1, pattern + 1, flags);
		}
		return 0;
	}

	if (*pattern == *name) {
		return match(name + 1, pattern + 1, flags);
	}

	return 0;
}