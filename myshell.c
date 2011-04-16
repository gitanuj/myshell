//      myshell.c
//      
//      Copyright 2011
//      
//      This program is free software; you can redistribute it and/or modify
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

#define MAX_USER 32
#define MAX_HOSTNAME 64
#define MAX_CWD 256
#define MAX_INPUT 1024
#define MAX_ARGS 10

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

int main()
{
	char c[MAX_INPUT];
	int i=0, flag=0;
	char *args[MAX_ARGS];
	pid_t pid;
	int num_of_args;
	
	//	Print welcome message
	init();
	
	//	Start with endless loop of shell
	while(1)
	{
		// Calculate stuff for control statement
		strcpy(USER, getenv("USER"));
		gethostname(HOSTNAME, MAX_HOSTNAME);
		strcpy(CWD, getenv("PWD"));
		
		//	Print control statement
		printControl();
		
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
							  printf("UA");
							  i-=2;
							}
							else if(c[i] == 66)
							{
							  // Down arrow key pressed
							  printf("DA");
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
						printf("BK");
						if(i > 0)
							i--;
						break;
	  
				default  : // Any other key pressed
						printf("%c", c[i]);
						i++;
			}
			if(flag)
			{
				flag = 0;
				break;
			}
		}
		
		// prepare *args[] for execvp()
		num_of_args = prepareArgs(c, args);
		
		if(strcmp(args[0], "exit") == 0)
		{
			// input is 'exit'
			exit(1);
		}
		else if(strcmp(args[0], "cd") == 0)
		{
			// input is 'cd'
			if(strcmp(args[1], "..") == 0)
			{
				// Go one directory up
			}
			else
			{
				// set "PWD" to args[1]
				setenv("PWD", args[1], 1);
			}
		}
		else
		{
			pid = fork();
			
			if(pid < 0)
			{
				// fork() unsuccessful
			}
			else if(pid > 0)
			{
				// parent
				wait(NULL);
			}
			else
			{
				// rewriting instruction set with c[]
				execvp(c, args);
			}
		}
		
		//	Reset input array
		i = 0;
	}
}
