//      myshell.c
//      A simple POSIX compliant shell
//      
//      Copyright 2011 Tanuj Mittal <tanuj.183@gmail.com>
//      Copyright 2011 Sahaj Biyani <sahajbiyani@gmail.com>
//      
//      myshell is free software; you can redistribute it and/or modify
//      it under the terms of the GNU General Public License as published by
//      the Free Software Foundation; either version 2 of the License, or
//      (at your option) any later version.
//      
//      This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY; without even the implied warranty of
//      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//      GNU General Public License for more details.
//      
//      You should have received a copy of the GNU General Public License
//      along with this program; if not, write to the Free Software
//      Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
//      MA 02110-1301, USA.


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>

#define MAX_USER 32
#define MAX_HOSTNAME 64
#define MAX_CWD 256
#define MAX_INPUT 1024
#define MAX_ARGS 10
#define MAX_HISTORY 100

char USER[MAX_USER], HOSTNAME[MAX_HOSTNAME], CWD[MAX_CWD];

void init()
{
	printf("Welcome to myshell !\n\t* http://www.github.com/free2talk/myshell\n\n");
}

char* getShortCWD()
{
	char *ptr = strstr(CWD, getenv("HOME"));
	int i,j;
	if(ptr == NULL || ptr != CWD)
		return CWD;
	else
	{
		ptr[0] = '~';
		for(i=strlen(getenv("HOME")), j=1 ; i<=strlen(CWD) ; i++,j++)
		{
			ptr[j] = CWD[i];
		}
		return ptr;
	}
}

void printControl()
{
	char c;
	if(strcmp(USER, "root") == 0)
		c = '#';
	else
		c = '$';
	printf("<myshell>%s@%s:%s%c ", USER, HOSTNAME, getShortCWD(), c);
}

int getch() 
{
	struct termios oldt,
				 newt;
	int            ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}

int prepareArgs(char *buffer, char *args[MAX_ARGS])
{
	char *temp = strtok(buffer, " ");
	int i=0;
	
	while(temp != NULL)
	{
		args[i] = temp;
		temp = strtok(NULL, " ");
		i++;
	}
	args[i] = NULL;
	
	return i;
}

// signal handler for SIGINT
void catch_int(int sig_num)
{
    signal(SIGINT, catch_int);
}

// signal handler for SIGUSR1
void catch_usr1(int sig_num)
{
    exit(1);
}

// add input to history
void addToHistory(char *c, char *history[MAX_HISTORY], int *n)
{
	int flag = 0;
	
	if(*n > 0)
	{
		if(strcmp(c, history[(*n)-1]) != 0)
		{
			flag = 1;
		}
	}
	else
	{
		flag = 1;
	}
	
	if(flag)
	{
		history[*n] = (char*)malloc(strlen(c));
		strcpy(history[*n], c);
		(*n)++;
	}
}

void getPreviousHistory(char *history[MAX_HISTORY], int history_count, int *count, int *i, char *c)
{
	int j,len,end;
	
	if(*count > 0)
	{
		if(*count == history_count)
			end = *i;
		else
			end = strlen(history[(*count)]);
		
		// get cursor to begining of line
		for(j=0 ; j < *i ; j++)
			putchar('\b');
			
		for(j=0 ; j < *i ; j++)
			putchar(' ');
			
		for(j=0 ; j < *i ; j++)
			putchar('\b');
		
		// put history line
		printf("%s", history[(*count)-1]);
		c = history[(*count)-1];
		
		len = strlen(history[(*count)-1]);
		
		*i = len;
		(*count)--;
	}
}

int main()
{
	char c[MAX_INPUT];
	int i=0, flag=0;
	char *args[MAX_ARGS];
	char *history[MAX_HISTORY];
	pid_t pid;
	int num_of_args;
	int history_count=0, count;
	
	//	Print welcome message
	init();
	
	// set SIGINT signal handler
	signal(SIGINT, catch_int);
	
	// set SIGUSR1 signal handler
	signal(SIGUSR1, catch_usr1);
	
	//	Start with endless loop of shell
	while(1)
	{
		// Calculate stuff for control statement
		strcpy(USER, getenv("USER"));
		gethostname(HOSTNAME, MAX_HOSTNAME);
		getcwd(CWD, MAX_CWD);
		
		//	Print control statement
		printControl();
		
		count = history_count;
		
		//	Get user input
		while((c[i] = (char)getch()))
		{
			switch(c[i])
			{
				case   9 : // Tab pressed
						break;
						  
				case  10 : // Enter pressed
						printf("\n");
						c[i] = '\0';
						
						//	Reset input array
						i = 0;
						
						flag = 1;
						break;
						  
				case  27 : 
						c[++i] = (char)getch();
						if(c[i] == 91)
						{
							c[++i] = (char)getch();
							if(c[i] == 65)
							{
							  // Up arrow key pressed
							  //printf("UA");
							  i-=2;
							  getPreviousHistory(history, history_count, &count, &i, c);
							}
							else if(c[i] == 66)
							{
							  // Down arrow key pressed
							  printf("DA");
							  //getNextHistory(history, history_count, count, i);
							  i-=2;
							}
							else if(c[i] == 67)
							{
							  // Right arrow key pressed
							  printf("RA");
							  i-=2;
							}
							else if(c[i] == 68)
							{
							  // Left arrow key pressed
							  printf("LA");
							  i-=2;
							}
							else
							{
							  // Any other special key pressed
							}
						}
						else
						{
							// Esc was pressed before this key
							printf("%c", c[i]);
							c[i-1] = c[i];
						}
						break;
						  
				case 127 : // Backspace
						if(i > 0)
						{
							putchar('\b');
							putchar(' ');
							putchar('\b');
							i--;
						}
						else
							i=0;
						break;
	  
				default  : // Any other key pressed
						printf("%c", c[i]);
						i++;
			}
			if(flag)
			{
				// Get out of input loop
				flag = 0;
				break;
			}
		}
		
		// if input is empty
		if(strlen(c) == 0)
			continue;
			
		// add c to history
		addToHistory(c, history, &history_count);
		
		// prepare *args[] for execvp()
		num_of_args = prepareArgs(c, args);
		
		if(strcmp(args[0], "cd") == 0)
		{
			// input is 'cd'
			if(chdir(args[1]) != 0)
			{
				perror("myshell: cd");
			}
		}
		else
		{
			// fork a child
			pid = fork();
			
			if(pid < 0)
			{
				// fork unsuccessful
			}
			else if(pid > 0)
			{
				// parent
				wait(NULL);
			}
			else
			{
				// child
				if(strcmp(args[0], "exit") == 0)
				{
					// input is 'exit'
					printf("exit\n");
					
					// send SIGUSR1 signal to parent
					kill(getppid(), SIGUSR1);
					
					exit(0);
				}
				else
				{
					// rewriting instruction set with c[]
					if(execvp(c, args) == -1)
					{
						printf("myshell: %s: %s\n", args[0], strerror(errno));
					}
					exit(0);
				}
			}
		}
	}
	
	return 0;
}
