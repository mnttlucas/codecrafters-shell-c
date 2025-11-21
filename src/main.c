#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256
#define CMDSIZE 16
#define NB_BUILTIN_COMMANDS 3

char builtin_commands[NB_BUILTIN_COMMANDS][CMDSIZE] = {"echo", "exit", "type"};
int run = 1;

int evaluate(char *cmd, char *res)
{
	int i;
	int ret = 1;
	
	if(!strncmp(cmd, "echo ", strlen("echo ")))
	{
		snprintf(res, BUFSIZE, "%s\n", cmd + strlen("echo "));
	}
	else if(!strncmp(cmd, "exit", strlen("exit")))
	{
		ret = 0;
		run = 0;
	}
	else if(!strncmp(cmd, "type ", strlen("type ")))
	{
		snprintf(res, BUFSIZE, "%s: not found\n", cmd + strlen("type "));
		for(i = 0; i < NB_BUILTIN_COMMANDS; i++)
		{
			if((!strncmp(builtin_commands[i], cmd + strlen("type "), strlen(builtin_commands[i]))))
			{
				snprintf(res, BUFSIZE, "%s is a shell builtin\n", cmd + strlen("type "));
			}
		}
	}
	else
	{
		snprintf(res, BUFSIZE, "%s: command not found\n", cmd);
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