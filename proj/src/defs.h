#ifndef MYDEFS
#define MYDEFS 1

# include <sys/stat.h>
# include <stdio.h> 
# include <string.h>
# include <stdlib.h>
# include <unistd.h>
# include <errno.h>
# include <error.h>

#define IGNORE_CASE 16
#define true 1
#define false 0
#define bool int

int optionl_stat (const char *name, struct stat *p);
int optionp_stat (const char *name, struct stat *p);

int get_statinfo (const char *pathname, const char *name, struct stat *p);

struct predicate;
struct options;

typedef bool (*PRED_FUNC)(const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr);

enum comparison_type {
    COMP_GT,
    COMP_LT,
    COMP_EQ
};

enum permissions_type {
    PERM_AT_LEAST,
    PERM_ANY,
    PERM_EXACT
};

enum predicate_type {
    NO_TYPE,
    PRIMARY_TYPE,
    UNI_OP,
    BI_OP,
    OPEN_PAREN,
    CLOSE_PAREN
};

enum predicate_precedence {
    NO_PREC,
    COMMA_PREC,
    OR_PREC,
    AND_PREC,
    NEGATE_PREC,
    MAX_PREC
};

struct perm_val {
    enum permissions_type kind;
    mode_t val[2];
};

struct size_val {
    enum comparison_type kind;
    int blocksize;
    int size;
};

enum file_type {
    FTYPE_BLK,
    FTYPE_CHR,
    FTYPE_DIR,
    FTYPE_REG,
    FTYPE_LNK,
    FTYPE_FIFO,
    FTYPE_SOCK,
    FTYPE_COUNT
};

struct predicate {
    PRED_FUNC pred_func;

    enum predicate_type p_type;
    const char *p_name;

    enum predicate_precedence p_prec;


    bool need_stat;
    bool need_type;
    bool need_inum;

    union {
        const char *str;
        bool types[FTYPE_COUNT];
    } args;

    struct predicate *pred_next;
    struct predicate *pred_left;
    struct predicate *pred_right;
};

enum SymlinkOption
{
    SYMLINK_NEVER_DEREF,
    SYMLINK_ALWAYS_DEREF,
    SYMLINK_DEREF_ARGSONLY
};

void set_follow_state (enum SymlinkOption opt);

enum arg_type
{
    ARG_OPTION,
    ARG_NOOP,
    ARG_POSITIONAL_OPTION,
    ARG_TEST,
    ARG_SPECIAL_PARSE,
    ARG_PUNCTUATION,
    ARG_ACTION
};

struct parser_table;

typedef bool (*PARSE_FUNC)(const struct parser_table *p, char *argv[], int *arg_ptr);

struct parser_table
{
    enum arg_type type;
    const char *parser_name;
    PARSE_FUNC parser_func;
    PRED_FUNC pred_func;
};

const struct parser_table* find_parser (const char *search_name);
bool parse_print (const struct parser_table*, char *argv[], int *arg_ptr);
bool parse_openparen (const struct parser_table* entry, char *argv[], int *arg_ptr);
bool parse_closeparen (const struct parser_table* entry, char *argv[], int *arg_ptr);


typedef bool PREDICATEFUNCTION(const char *pathname, struct stat *stat_buf, struct predicate *pred_ptr);

PREDICATEFUNCTION pred_and;
PREDICATEFUNCTION pred_closeparen;
PREDICATEFUNCTION pred_comma;
PREDICATEFUNCTION pred_empty;
PREDICATEFUNCTION pred_group;
PREDICATEFUNCTION pred_iname;
PREDICATEFUNCTION pred_name;
PREDICATEFUNCTION pred_negate;
PREDICATEFUNCTION pred_openparen;
PREDICATEFUNCTION pred_or;
PREDICATEFUNCTION pred_print;
PREDICATEFUNCTION pred_readable;
PREDICATEFUNCTION pred_size;
PREDICATEFUNCTION pred_type;
PREDICATEFUNCTION pred_user;


/* tree.c */ 
struct predicate *build_expression_tree (int argc, char *argv[], int end_of_leading_options);
struct predicate *get_eval_tree (void);
struct predicate *get_new_pred (const struct parser_table *entry);
struct predicate *get_new_pred_chk_op (const struct parser_table *entry, const char *arg);
void free_predicates ();

/* util.c */
int process_leading_options (int argc, char *argv[]);
void set_option_defaults (struct options *p);
void usage (int status);
bool apply_predicate (const char *pathname, struct stat *stat_buf, struct predicate *p);
bool looks_like_expression (const char *arg, bool leading);
struct predicate *insert_primary_withpred (const struct parser_table *entry, PRED_FUNC fptr, const char *arg);
void cleanup();

struct options
{
    bool do_dir_first;
    bool explicit_depth;
    int maxdepth;
    int mindepth;
    bool stay_on_filesystem;

    enum SymlinkOption symlink_handling;

    int (*xstat) (const char *name, struct stat *statbuf);

    int (*x_getfilecon) (int, const char *, char **);
};

struct state
{
    int curdepth;

    bool have_stat;

    bool have_type;

    mode_t type;

    const char *rel_pathname;

    int cwd_dir_fd;

    int starting_path_length;

    bool stop_at_current_level;
    int exit_status;
};

/* matcher.c */
int match(const char *name, const char *pattern, int flags);

/* find.c */ 
bool process_all_startpoints(int argc, char *argv[]);

/* main.c */
extern const char *program_name;
extern struct options options;
extern struct state state;

#endif // MYDEFS