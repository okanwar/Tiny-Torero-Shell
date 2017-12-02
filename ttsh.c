/*
 * The Tiny Torero Shell (TTSH)
 *
 * Add your top-level comments here.
 */

// NOTE: all new includes should go after the following #define
#define _XOPEN_SOURCE 600
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/wait.h>

#include "parse_args.h"
#include "history_queue.h"



// TODO: add your function prototypes here as necessary
void add_queue(char *cmdline);
void reapHandler();
void readArgs(char **argsv, int *background_flag, int *chdir_flag, int *parse_again);
void terminate(char **argv);
void runCommand(char **argv);
int isHistoryCmd( char **str );
char *addStrings( char *str1, char *str2);


int main() { 
	signal(SIGUSR1, reapHandler);
	int background_flag = 0;
	int chrdir_flag = 0;
	int parse_again = 0;
	char *argv[MAXARGS];
	struct sigaction sa;
	sa.sa_handler = reapHandler;
	sa.sa_flags = SA_NOCLDSTOP;
	sigaction(SIGCHLD, &sa, NULL);

	while(1) {
		chrdir_flag = 0;
		parse_again = 0;
		// (1) print the shell prompt
		fprintf(stdout, "ttsh> ");  
		fflush(stdout);

		// (2) read in the next command entered by the user
		char cmdline[MAXLINE];
		if ((fgets(cmdline, MAXLINE, stdin) == NULL)
				&& ferror(stdin)) {
			// fgets could be interrupted by a signal.
			// This checks to see if that happened, in which case we simply
			// clear out the error and restart our loop so it re-prompts the
			// user for a command.
			clearerr(stdin);
			continue;
		}

		/*
		 * Check to see if we reached the end "file" for stdin, exiting the
		 * program if that is the case. Note that you can simulate EOF by
		 * typing CTRL-D when it prompts you for input.
		 */
		if (feof(stdin)) {
			fflush(stdout);
			exit(0);
		}

		//Put command in history
		add_queue(cmdline);

		// TODO: complete top-level steps
		// (3) make a call to parseArguments function to parse it into its argv
		// format
		background_flag = parseArguments(cmdline,argv);
		if((argv[0] == NULL) || (chrdir_flag == 1)){ continue; }
		readArgs(argv, &background_flag, &chrdir_flag, &parse_again);
		if( parse_again ) {readArgs(argv, &background_flag, &chrdir_flag, &parse_again);}
		terminate(argv);

		// (4) Call a function that will determine how to execute the command
		// that the user entered, and then execute it
		int child_pid = fork();
		if (child_pid == 0) {
			if( strcmp(argv[0], "history") == 0){
				print_history();
				exit(0);
			} else {
				runCommand(argv);
			}
		}
		else {
			if(background_flag == 0) {
				if( waitpid(child_pid, NULL, 0) == -1 ){
					printf("Error1: Child not reaped properly\n");
				}
			}
		}
	}
	return 0;
}


void reapHandler() {
	//Wait for processes
	while( (waitpid(-1, NULL, WNOHANG)) > 0 ){
	}
}
void runCommand(char **argv) {
	int ret = execvp(*argv, argv);
	if (ret == -1) {
		printf("ERROR: Command not found\n");
		exit(1);
	}
	exit(0);
}

int isHistoryCmd( char **str){
	int num = 0;
	if( str[0][0] != ('!')){
		return -1;
	} else {
		num = strtol( str[0]+1, NULL, 10);
		return num;
	}
}

void terminate(char **argv) {
	if(strcmp(argv[0], "exit") == 0 ) {
		printf("Exiting Tiny Torero Shell\n");
		exit(0);
	}
}

void readArgs(char **argv, int *background_flag, int *chrdir_flag, int *parse_again) {
	int ret = 0;
	char *cmd_string = NULL;
	printf("%s\n", argv[0]);
	if((argv[1] == NULL) && ((ret = isHistoryCmd(argv)) != -1) ){
		printf("ret:%d\n", ret);
		 cmd_string = check_history(ret);
		 printf("CMD:%s\n", cmd_string);
		 if( cmd_string == NULL){
			 printf("Command not found in history\n");
		 }
		 else {
			*background_flag = parseArguments( cmd_string, argv);
			if( strcmp( argv[0], "cd") == 0 ){
			   *parse_again = 1;
		  	}	   
		 }
	}
	else if( strcmp( argv[0], "cd" ) == 0){
		*chrdir_flag = 1;
		if (argv[1] == NULL) {
		   chdir(getenv("HOME"));
		   return;
		} else {
			char cwd[4000];
			getcwd( cwd, sizeof(cwd));
			char *dir = malloc(1 + strlen(cwd) + strlen(argv[1]) );
			strcpy( dir, cwd);
			strcat( dir, argv[1] );
			if( chdir( dir ) == -1 ){
				printf("Invalid directory\n");
			}
		}
 	}		
}

char *addStrings( char *str1, char *str2){
	char *str3 = malloc(1 + strlen(str1) + strlen(str2) );
	strcpy( str3, str1 );
	strcat( str3, str2 );
	return str3;
}
