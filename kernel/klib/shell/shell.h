typedef void (*shell_func_t)(int argc, char** argv);

typedef struct shell_command {
    const char* name;
    const char* usage;
    const char* desc;
    shell_func_t func;
    int expected_args;
} shell_command_t;

#define MAX_COMMANDS 64

static shell_command_t command_list[MAX_COMMANDS];
static int command_count = 0;


void shell_init();
void shell_start();
void help_cmd(int argc, char** argv);
void shell_add(shell_func_t func, const char* name, const char* usage, const char* desc, int expected_args);