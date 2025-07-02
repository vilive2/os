#include <assert.h>

#include "defs.h"

static struct predicate *predicates = NULL;
static struct predicate *eval_tree = NULL;
static struct predicate *last_pred = NULL;
static char **start_points;
static size_t num_start_points = 0;

static struct predicate *scan_rest(struct predicate **input,
				   struct predicate *head, short int prev_prec);

static struct predicate *get_expr(struct predicate **input, short int prev_prec,
				  const struct predicate *prev_pred) {
	struct predicate *next = NULL;
	struct predicate *this_pred = (*input);

	if (*input == NULL) {
		cleanup();
		error(EXIT_FAILURE, 0, "invalid expression");
	}

	switch (this_pred->p_type) {
		case NO_TYPE:
		case BI_OP:
		case CLOSE_PAREN:
			cleanup();
			error(EXIT_FAILURE, 0, "invalid expression");
			break;
		case PRIMARY_TYPE:
			next = *input;
			*input = (*input)->pred_next;
			break;

		case UNI_OP:
			next = *input;
			*input = (*input)->pred_next;
			next->pred_right = get_expr(input, NEGATE_PREC, next);
			break;
		case OPEN_PAREN:
			if (NULL == (*input)->pred_next) {
				cleanup();
				error(EXIT_FAILURE, 0, "invalid expression");
			}

			prev_pred = *input;
			*input = (*input)->pred_next;
			if ((*input)->p_type == CLOSE_PAREN) {
				cleanup();
				error(EXIT_FAILURE, 0,
				      "invalid expression; empty parentheses "
				      "are not allowed.");
			}
			next = get_expr(input, NO_PREC, prev_pred);
			if (*input == NULL || (*input)->p_type != CLOSE_PAREN) {
				cleanup();
				error(EXIT_FAILURE, 0,
				      "invalid expression; I was expecting to "
				      "find a ')' somewhere but did not see "
				      "one.");
			}
			*input = (*input)->pred_next;
			break;
		default:
			cleanup();
			error(EXIT_FAILURE, 0, "invalid expression");
	}

	if (*input == NULL)
		return next;
	if ((int)(*input)->p_prec > (int)prev_prec) {
		next = scan_rest(input, next, prev_prec);
		if (next == NULL) {
			cleanup();
			error(EXIT_FAILURE, 0, "invalid expression");
		}
	}

	return next;
}

static struct predicate *scan_rest(struct predicate **input,
				   struct predicate *head,
				   short int prev_prec) {

	struct predicate *tree;
	if ((*input == NULL) || ((*input)->p_type == CLOSE_PAREN))
		return NULL;
	tree = head;
	while ((*input != NULL) && ((int)(*input)->p_prec) > (int)prev_prec) {
		switch ((*input)->p_type) {
			case NO_TYPE:
			case PRIMARY_TYPE:
			case UNI_OP:
			case OPEN_PAREN:
				cleanup();
				error(EXIT_FAILURE, 0, "invalid expression");
				break;
			case BI_OP:
				struct predicate *prev = *input;
				(*input)->pred_left = tree;
				tree = *input;
				*input = (*input)->pred_next;
				tree->pred_right =
				    get_expr(input, tree->p_prec, prev);
				break;
			case CLOSE_PAREN:
				return tree;
			default:
				cleanup();
				error(EXIT_FAILURE, 0, "invalid expression");
		}
	}

	return tree;
}

struct predicate *get_eval_tree(void) {
	return eval_tree;
}

struct predicate *build_expression_tree(int argc, char *argv[],
					int end_of_leading_options) {
	const struct parser_table *parse_entry;
	char *predicate_name;
	struct predicate *cur_pred;
	const struct parser_table *entry_close, *entry_print, *entry_open;
	int i, oldi;

	predicates = NULL;

	start_points = argv + end_of_leading_options;
	for (i = end_of_leading_options;
	     i < argc && !looks_like_expression(argv[i], true); i++) {
		num_start_points++;
	}

	entry_open = find_parser("(");
	entry_close = find_parser(")");
	entry_print = find_parser("print");
	assert(entry_open != NULL);
	assert(entry_close != NULL);
	assert(entry_print != NULL);

	parse_openparen(entry_open, argv, &argc);
	last_pred->p_name = "(";

	while (i < argc) {
		// error check current arg doesn't seem to expr.
		if (!looks_like_expression(argv[i], false)) {
			cleanup();
			error(0, 0, "paths must precede expression: `%s`",
			      argv[i]);
			exit(EXIT_FAILURE);
		}

		predicate_name = argv[i];
		parse_entry = find_parser(predicate_name);
		// error check parser entry not found
		if (parse_entry == NULL) {
			cleanup();
			error(EXIT_FAILURE, 0, "unknown predicate `%s`",
			      predicate_name);
		}

		i++;
		oldi = i;
		if (!(*(parse_entry->parser_func))(parse_entry, argv, &i)) {
			// error
			cleanup();
			error(EXIT_FAILURE, 0, "invalid argument to `%s`",
			      predicate_name);
		} else {
			last_pred->p_name = predicate_name;
		}
	}

	if (predicates->pred_next == NULL) {
		cur_pred = predicates;
		predicates = last_pred = predicates->pred_next;
		free(cur_pred);
	} else {
		parse_closeparen(entry_close, argv, &argc);
		last_pred->p_name = ")";
	}

	parse_print(entry_print, argv, &argc);
	last_pred->p_name = "-print";

	cur_pred = predicates;
	eval_tree = get_expr(&cur_pred, NO_PREC, NULL);

	return eval_tree;
}

struct predicate *get_new_pred(const struct parser_table *entry) {
	struct predicate *new_pred;
	(void)entry;

	assert(entry->type != ARG_OPTION);

	new_pred = (struct predicate *)malloc(sizeof(struct predicate));

	if (new_pred == NULL) {
		cleanup();
		error(EXIT_FAILURE, 0, "memory allocation failed");
	}

	if (predicates == NULL)
		last_pred = predicates = new_pred;
	else {
		last_pred->pred_next = new_pred;
		last_pred = new_pred;
	}

	last_pred->p_type = NO_TYPE;
	last_pred->p_prec = NO_PREC;
	last_pred->need_stat = true;
	last_pred->need_type = true;
	last_pred->pred_next = NULL;
	return last_pred;
}

struct predicate *get_new_pred_chk_op(const struct parser_table *entry,
				      const char *arg) {
	struct predicate *new_pred;
	static const struct parser_table *entry_and = NULL;

	if (NULL == entry_and)
		entry_and = find_parser("and");

	assert(entry_and != NULL);

	if (last_pred) {
		switch (last_pred->p_type) {
			case NO_TYPE:
				cleanup();
				error(EXIT_FAILURE, 0,
				      "invalid default insertion of and");
				break;
			case PRIMARY_TYPE:
			case CLOSE_PAREN:
				new_pred = get_new_pred(entry_and);
				new_pred->pred_func = pred_and;
				new_pred->p_type = BI_OP;
				new_pred->p_prec = AND_PREC;
				new_pred->need_stat = false;
				new_pred->need_type = false;
				new_pred->args.str = NULL;
				break;
			default:
				break;
		}
	}

	new_pred = get_new_pred(entry);
	return new_pred;
}

void free_predicates() {
	struct predicate *cur = predicates;
	while (cur != NULL) {
		struct predicate *next = cur->pred_next;
		free(cur);
		cur = next;
	}
}