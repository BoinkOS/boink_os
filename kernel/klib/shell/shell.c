#include "../console/console.h"
#include "shell.h"
#include "../../utils.h"
#include "../../cpu/user_switch.h"
#include <stddef.h>


void shell_init() {
	console_set_background_color(0x9019ff);
	console_print("\n\n~~~ Welcome to BoinkOS! ~~~             moof!");
	console_set_color(0xc98fff);
	console_print("\n~ where there is a shell, there is a way... ~");
	console_set_background_color(0x000000);
	console_set_color(0xFFFFFF);
	console_println("\n");
	
	shell_add(help_cmd, "help", "help", "Show this command list", 0, 0);

	console_set_color(0x9019ff);
	console_println("\n\nBoinkOS Kernel Shell");
	console_set_color(0xFFFFFF);
	console_println("\n");

	kernel_ctx.ret_addr = shell_start;
	shell_start();
}


void shell_start() {
	char input[128];
	while (1) {
		console_set_color(0xFFFF00);
		console_print("boink$ ");
		console_set_color(0xFFFFFF);
		console_input(input, sizeof(input));
		
		char* argv[64];
		int argc;
		
		// parse input into argv[]
		argc = 0;
		char* token = strtok(input, " ");
		while (token && argc < 64) {
			argv[argc++] = token;
			token = strtok(NULL, " ");
		}
		if (argc == 0) continue;

		char* cmd_name = argv[0];


		int found = 0;
		for (int i = 0; i < command_count; i++) {
			if (strcmp(command_list[i].name, cmd_name) == 0) {
				found = 1;
				if ((argc - 1) < command_list[i].min_args || (command_list[i].max_args != -1 && (argc - 1) > command_list[i].max_args)) {
					console_set_color(0xFF0000);
					console_print("Usage: ");
					console_println(command_list[i].usage);
					console_set_color(0xFFFFFF);
				} else {
					command_list[i].func(argc, argv);
				}
				break;
			}
		}
		if (!found) {
			console_set_color(0xFF0000);
			console_print("Unknown command: ");
			console_println(cmd_name);
			console_set_color(0xFFFFFF);
		}
		console_putc('\n');
	}
}


void shell_add(shell_func_t func, const char* name, const char* usage, const char* desc, int min_args, int max_args) {
	if (command_count >= MAX_COMMANDS) {
		console_set_background_color(0xFF0000);
		console_set_color(0xFFFFFF);
		console_print("Unable to register command \"");
		console_print(name);
		console_print("\". Exceeds maximum command limit.");
		console_putc('\n');
		console_set_color(0xFFFFFF);
		console_set_background_color(0x000000);
		return;
	}

	shell_command_t cmd = {
		.name = name,
		.usage = usage,
		.desc = desc,
		.func = func,
		.min_args = min_args,
		.max_args = max_args
	};

	command_list[command_count++] = cmd;
}

void help_cmd(int argc, char** argv) {
	console_println("BoinkOS Kernel Shell -- Help");
	console_println("Available commands:");
	for (int i = 0; i < command_count; i++) {
		console_print(command_list[i].usage);
		console_print(" - ");
		console_println(command_list[i].desc);
	}
}
