#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

typedef struct {
	char **items;
	size_t count;
	size_t capacity;
} da_t;

#define BASE_CAPACITY 4
#define BUFSIZE 2048
#define CMDSIZE 16
#define CMDPATHSIZE 128
#define NB_BUILTIN_COMMANDS 4

#define CMD_FAILURE -1
#define CMD_EXIT 0
#define CMD_SUCCESS 1

#define CMD_NOT_FOUND 0
#define CMD_FOUND 1

char builtin_commands[NB_BUILTIN_COMMANDS][CMDSIZE] = {"echo", "exit", "pwd", "type"};
int run = 1;

void da_append(da_t *da, char *item)
{
	if(da -> count >= da -> capacity)
	{

		da -> capacity *= 2;
		da -> items = realloc(da -> items, da -> capacity * sizeof(*(da -> items)));
	}

	da -> items[(da -> count)++] = strdup(item);
}

void da_init(da_t *da)
{
	da -> count = 0;
	da -> capacity = BASE_CAPACITY;
	da -> items = malloc(da -> capacity * sizeof(*(da -> items)));
}

void da_free(da_t *da)
{
	int i;

	for(i = 0; i < da -> count; i++)
	{
		free((da -> items)[i]);
	}
}

void tokenize_cmd(da_t *da, char *cmd)
{
	char *tmp, *tok;
	tmp = strdup(cmd);
	tok = strtok(tmp, " ");

	while(tok)
	{
		da_append(da, tok);
		tok = strtok(NULL, " ");
	}
	
	free(tmp);
}

int cmd_exists(char *cmd, char *res)
{
	char *dir, *path;

	path = strdup(getenv("PATH"));
	if(path)
	{
		dir = strtok(path, ":");
		while(dir)
		{
			snprintf(res, BUFSIZE, "%s/%s", dir, cmd);
			if(!access(res, X_OK))
			{
				free(path);
				return(CMD_FOUND);
			}
			dir = strtok(NULL, ":");
		}
		free(path);
	}

	return(CMD_NOT_FOUND);
}

int execute_echo(char *cmd, char *res)
{
	snprintf(res, BUFSIZE, "%s\n", cmd + 5);
	return(CMD_SUCCESS);
}

int execute_exit()
{
	run = 0;
	return(CMD_EXIT);
}

int execute_pwd(char *res)
{
	if(getcwd(res, BUFSIZE))
	{
		res[strlen(res) + 1] = '\0';
		res[strlen(res)] = '\n';
		return(CMD_SUCCESS);
	}
	else
	{
		return(CMD_FAILURE);
	}
}

int execute_type(da_t *da, char *res)
{
	char relative[CMDPATHSIZE];
	int i;

	for(i = 0; i < NB_BUILTIN_COMMANDS; i++)
	{
		if((!strncmp(builtin_commands[i], (da -> items)[1], strlen(builtin_commands[i]))))
		{
			snprintf(res, BUFSIZE, "%s is a shell builtin\n", builtin_commands[i]);
			return(CMD_SUCCESS);
		}
	}

	if(cmd_exists((da -> items)[1], relative))
	{
		snprintf(res, BUFSIZE, "%s is %s\n", (da -> items)[1], relative);
		return(CMD_SUCCESS);
	}

	snprintf(res, BUFSIZE, "%s: not found\n", (da -> items)[1]);
	return(CMD_SUCCESS);
}

int evaluate(char *cmd, char *res)
{
	char **args, path[CMDPATHSIZE];
	da_t da;
	int i;
	int ret, status;
	pid_t pid;

	da_init(&da);
	tokenize_cmd(&da, cmd);
	
	if(!strncmp((da.items)[0], "echo", strlen("echo")))
	{
		if(da.count >= 2)
		{
			ret = execute_echo(cmd, res);
		}
		else
		{
			fprintf(stderr, "[!] Usage : echo xyz\n");
			ret = CMD_FAILURE;
		}
	}
	else if(!strncmp((da.items)[0], "exit", strlen("exit")))
	{
		if(da.count == 1 || da.count == 2)
		{
			ret = execute_exit();
		}
		else
		{
			fprintf(stderr, "[!] Usage : exit [code]\n");
			ret = CMD_FAILURE;
		}
	}
	else if(!strncmp((da.items)[0], "pwd", strlen("pwd")))
	{
		if(da.count == 1)
		{
			ret = execute_pwd(res);
		}
		else
		{
			fprintf(stderr, "[!] Usage : pwd\n");
			ret = CMD_FAILURE;
		}
	}
	else if(!strncmp((da.items)[0], "type", strlen("type")))
	{
		if(da.count == 2)
		{
			ret = execute_type(&da, res);
		}
		else
		{
			fprintf(stderr, "[!] Usage : type command\n");
			ret = CMD_FAILURE;
		}
	}
	else
	{
		if(cmd_exists((da.items)[0], path))
		{
			args = (char **) malloc((da.count + 1) * sizeof(*(da.items)));
			for(i = 0; i < da.count; i++)
			{
				args[i] = strdup((da.items)[i]);
			}
			args[i] = NULL;
			
			pid = fork();

			if(pid == 0)
			{
				execv(path, args);
			}
			else if(pid > 0)
			{
				waitpid(pid, &status, 0);
				ret = CMD_SUCCESS;
			}
			else
			{
				fprintf(stderr, "[!] Error : fork()");
				ret = CMD_FAILURE;
			}
		}
		else
		{
			snprintf(res, BUFSIZE, "%s: command not found\n", da.items[0]);
			ret = CMD_SUCCESS;
		}
	}

	da_free(&da);
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
		if(evaluate(input_buf, output_buf) == CMD_SUCCESS)
		{
			/* Print */
			printf("%s", output_buf);
		}
	}
}

int main(int argc, char **argv)
{
	setbuf(stdout, NULL);
	repl();
	return(0);
}