#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 256

int main(int argc, char **argv) {
	char input_buf[BUFSIZE];

	setbuf(stdout, NULL);

	printf("$ ");
	fgets(input_buf, BUFSIZE, stdin);
	input_buf[strlen(input_buf) - 1] = '\0';
	printf("%s: command not found\n", input_buf);

	return(0);
}