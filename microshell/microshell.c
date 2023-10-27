#include <stdio.h>
#include <stdlib.h> 
#include <dirent.h> 
#include <pwd.h>
#include <sys/fcntl.h> 
#include <string.h> 
#include <unistd.h> 
#include <sys/wait.h>
#include <errno.h>
#include <time.h>



#define TRUE 1
#define PINK "\e[38;5;201m"
#define CLEAR "\e[0m"
#define LIME "\e[38;5;10m"
#define RED "\e[38;5;1m"
#define ORANGE "\e[38;5;202m"
#define CYAN "\e[5;142m"
char location[2048];
int parsstr(char* str, char* abc[])
{
	int count = 0;
	char* temp = strtok(str, " \n");
	while (temp != NULL)
	{
		abc[count] = temp;
		temp = strtok(NULL, " \n");
		count++;
	}
	abc[count] = NULL;
	return count;
}
void showlocation()
{
	char* usr = getpwuid(getuid())->pw_name;
	getcwd(location, 2048);
	time_t t = time(NULL);
	struct tm tm = *localtime(&t);
	char minutes[] = "", secs[] = "";
	if (tm.tm_min < 10)
		strcat(minutes, "0");
	if (tm.tm_sec < 10)
		strcat(secs, "0");
	printf("["CYAN"%d:%s%d:%s%d"CLEAR"]["ORANGE"%s:"PINK" %s"CLEAR"]"LIME"$ "CLEAR, tm.tm_hour, minutes, tm.tm_min, secs, tm.tm_sec, usr, location);
}
int main()
{
	while (TRUE)
	{
		showlocation();
		char* directory;
		int cd = 0;
		char str[2048];
		fgets(str, 2048, stdin);

		char* args[128];
		int count = parsstr(str, args);
		if (count == 0)
		{
			printf(RED"ERROR\n"CLEAR);
		}
		else if (strcmp(args[0], "help") == 0)
		{
			printf("Autor - Aliaksandr Halaunia\n---------------------------\nCOMMANDS:\nhelp\nexit\npwd\ntouch\ncd\ncat\nhead\nwhoami\nclear\nrm\n---------------------------\n");
		}
		else if (strcmp(args[0], "pwd") == 0)
		{
			printf("%s\n", location);
		}
		else if (strcmp(args[0], "exit") == 0)
		{
			exit(0);
		}
		else if (args[0][0] == '.')
		{
			int id = fork();
			if (id == 0)
			{
				execvp(args[0], args);
			}
			else
			{
				wait(NULL);
			}
		}
		else if (strcmp(args[0], "whoami") == 0)
		{
			char* s;
			s = getenv("USER");
			printf("%s\n", s);
		}
		else if (strcmp(args[0], "cd") == 0)
		{
			directory = args[1];
			cd = chdir(directory);
			if (cd)
			{
				if (args[1])
				{
					if (strcmp(args[1], "~") == 0)
					{
						chdir(getenv("HOME"));
					}
					else
					{
						int error = errno;
						fprintf(stderr, "*** No directory('%s') errno=%d: %s\n", directory, error, strerror(error));
					}
				}
				else
					chdir(getenv("HOME"));
			}
			else
			{
				char wd[1024] = "";
				getcwd(wd, sizeof(wd));
				fprintf(stderr, "('%s') <<>> %s\n", directory, wd);
			}
		}
		else if (strcmp(args[0], "touch") == 0)
		{
			FILE* file;
			file = fopen(args[1], "w");
			fclose(file);
		}
		else if (strcmp(args[0], "cat") == 0)
		{
			FILE* file = fopen(args[1], "r");
			if (file)
			{
				char a;
				while ((a = getc(file)) != EOF)
				{
					printf("%c", a);
				}
			}
			else
			{
				printf(RED"No file\n"CLEAR);
			}
		}
		else if (strcmp(args[0], "clear") == 0)
		{
			printf("\e[1;1H\e[2J");
		}
		else if (strcmp(args[0], "rm") == 0)
		{
			if (count == 1)
			{
				printf(RED"No file\n"CLEAR);
				break;
			}
			FILE* file = fopen(args[1], "r");
			if (file)
			{
				remove(args[1]);
			}
			else 
			{
				printf(RED"No file\n"CLEAR);
			}
		}
		else if (strcmp(args[0], "head") == 0)
		{
			FILE* file = fopen(args[1], "r");
			int lines_to_show = 10;
			if (count > 2)
				lines_to_show = atoi(args[2]);
			if (file)
			{
				char a;
				int line_count = 0;
				char line[1024] = "";
				while ((a = getc(file)) != EOF)
				{
					strncat(line, &a, 1);
					if (a == '\n')
					{
						line_count++;
						printf("%s", line);
						if (line_count < lines_to_show)
							memset(line, 0, strlen(line));
						else
						{
							break;
						}
					}
				}
			}
			else
			{
				printf(RED"No file\n"CLEAR);
			}
		}
		else
		{
			printf(RED"ERROR\n"CLEAR);
		}
	}
	return 0;
}

