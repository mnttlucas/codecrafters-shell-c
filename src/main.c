#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256

int run = 1;

int evaluate(char *cmd, char *res)
{
	int ret = 1;
	
	if(!strncmp(cmd, "echo ", strlen("echo ")))
	{
		snprintf(res, BUFSIZE, "%s\n", cmd + 5);
	}
	else if(!strncmp(cmd, "exit", strlen("exit")))
	{
		ret = 0;
		run = 0;
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