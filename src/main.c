#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256

void repl(void)
{
	/* Loop */
	while(1)
	{
		char input_buf[BUFSIZE];
	
		/* Read */
		printf("$ ");
		fgets(input_buf, BUFSIZE, stdin);
		input_buf[strlen(input_buf) - 1] = '\0';
	
		/* Eval */
		
		/* Print */
		printf("%s: command not found\n", input_buf);
	}
}

int main(int argc, char **argv) {
	setbuf(stdout, NULL);

	repl();

	return(0);
}