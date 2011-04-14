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

void init()
{
	printf("Welcome to myshell !\n\t* http://www.github.com/free2talk/myshell\n\n");
}

int main(int argc, char **argv)
{
	init();
	char hostname[50], command[100], CWD[200], CWD_SHORT[100];
	gethostname(hostname, 50);
	strcpy(CWD, getenv("PWD"));
	strcpy(CWD_SHORT, strncpy(strstr(CWD,getenv("HOME")), "~", strlen(getenv("HOME"))));
	printf("%s@%s:%s$ ", getenv("USER"), hostname, CWD_SHORT);
	while(gets(command))
	{
		if(strcmp(command, "exit")==0)
			exit(1);
		printf("%s@%s:%s$ ", getenv("USER"), hostname, CWD_SHORT);
	}
	return 0;
}

