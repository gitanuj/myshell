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
#include <stdlib.h>

#define MAX_USER 32
#define MAX_HOSTNAME 64
#define MAX_CWD 256
#define MAX_INPUT 1024

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
	printf("%s@%s:%s%c ", USER, HOSTNAME, getShortCWD(), c);
}

int main()
{
	char c[MAX_INPUT];
	int i=0;
	
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
		while((c[i]=getchar())!=10)
		{
			i++;
		}
		c[i] = '\0';
		
		// input is 'exit'
		if(strcmp(c, "exit") == 0)
		{
			exit(1);
		}
		
		//	Reset input array
		i = 0;
	}
}
