#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 256
#define CMDSIZE 16
#define NB_BUILTIN_COMMANDS 3

#define CMD_EXIT 0
#define CMD_SUCCESS 1

char builtin_commands[NB_BUILTIN_COMMANDS][CMDSIZE] = {"echo", "exit", "type"};
int run = 1;

int execute_echo(char *cmd, char *res)
{
	snprintf(res, BUFSIZE, "%s\n", cmd + strlen("echo "));
	return(CMD_SUCCESS);
}

int execute_exit()
{
	run = 0;
	return(CMD_EXIT);
}

int execute_type(char *cmd, char *res)
{
	char *dir, *path, relative[BUFSIZE];
	int i;

	for(i = 0; i < NB_BUILTIN_COMMANDS; i++)
	{
		if((!strncmp(builtin_commands[i], cmd + strlen("type "), strlen(builtin_commands[i]))))
		{
			snprintf(res, BUFSIZE, "%s is a shell builtin\n", cmd + strlen("type "));
			return(CMD_SUCCESS);
		}
	}

	path = strdup(getenv("PATH"));
	if(path)
	{
		dir = strtok(path, ":");
		while(dir)
		{
			snprintf(relative, BUFSIZE, "%s/%s", dir, cmd + strlen("type "));
			if(!access(relative, X_OK))
			{
				snprintf(res, BUFSIZE, "%s is %s\n", cmd + strlen("type "), relative);
				free(path);
				return(CMD_SUCCESS);
			}
			dir = strtok(NULL, ":");
		}
		free(path);
	}

	snprintf(res, BUFSIZE, "%s: not found\n", cmd + strlen("type "));
	return(CMD_SUCCESS);
}

int evaluate(char *cmd, char *res)
{
	int ret;
	
	if(!strncmp(cmd, "echo ", strlen("echo ")))
	{
		ret = execute_echo(cmd, res);
	}
	else if(!strncmp(cmd, "exit", strlen("exit")))
	{
		ret = execute_exit();
	}
	else if(!strncmp(cmd, "type ", strlen("type ")))
	{
		ret = execute_type(cmd, res);
	}
	else
	{
		snprintf(res, BUFSIZE, "%s: command not found\n", cmd);
		ret = CMD_SUCCESS;
	}

	return(ret);
}

void repl(void)
{
	char input_buf[BUFSIZE];
	char output_buf[BUFSIZE];

	/* Loop */
	while(run)
	{
		/* Read */
		printf("$ ");
		fgets(input_buf, BUFSIZE, stdin);
		input_buf[strlen(input_buf) - 1] = '\0';
	
		/* Eval */
		if(evaluate(input_buf, output_buf))
		{
			/* Print */
			printf("%s", output_buf);
		}
	}
}

int main(int argc, char **argv) {
	setbuf(stdout, NULL);

	repl();

	return(0);
}