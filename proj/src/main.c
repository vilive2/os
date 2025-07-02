#include <fcntl.h>
#include "defs.h"

const char *program_name = NULL;
struct options options;
struct state state;

int main(int argc, char *argv[]) {
	int end_of_leading_options = 0;

	if (argv[0])
		program_name = argv[0];
	else
		program_name = "myfind";

	state.exit_status = EXIT_SUCCESS;
	state.cwd_dir_fd = AT_FDCWD;

	set_option_defaults(&options);

	end_of_leading_options = process_leading_options(argc, argv);

	build_expression_tree(argc, argv, end_of_leading_options);

	process_all_startpoints(argc - end_of_leading_options,
				argv + end_of_leading_options);

	cleanup();

	return state.exit_status;
}
