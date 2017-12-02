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


//Flag struct
struct Flags{
	int background_flag;
	int chdir_flag;
	int history;
	int parse_again;
};
typedef struct Flags Flags;


// TODO: add your function prototypes here as necessary
void add_queue(char *cmdline);
void reapHandler();
void readArgs(char **argsv, Flags *flags);
void terminate(char **argv);
void runCommand(char **argv);
int isHistoryCmd( char **str );
char *addStrings( char *str1, char *str2);
void resetPointers(Flags *flags);
void initFlags(Flags *flags);

int main() { 
	signal(SIGUSR1, reapHandler);
	Flags flags;
	initFlags(&flags);
	char *argv[MAXARGS];
	struct sigaction sa;
	sa.sa_handler = reapHandler;
	sa.sa_flags = SA_NOCLDSTOP;
	sigaction(SIGCHLD, &sa, NULL);

	while(1) {
		resetPointers( &flags );
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

		//Parse arguments
		flags.background_flag = parseArguments(cmdline,argv);
		if((argv[0] == NULL) || (flags.chdir_flag == 1)){ continue; }
		readArgs(argv, &flags);
		if( flags.parse_again ) {readArgs(argv, &flags);}
		//Check if command was exit
		terminate(argv);

		//Fork and execute command
		int child_pid = fork();
		if (child_pid == 0) {		//Child
			if( flags.history ){
				exit(0);
			} else {
				runCommand(argv);
			}
		}
		else {						//Parent
			if(flags.background_flag == 0) {
				if( waitpid( -1, NULL, 0) < 0){
					printf("Error: Child not reaped properly\n");
				}
			}
		}
	}
	return 0;
}


/*
 * A handler to reap zombie processes
 */
void reapHandler() {
	//Wait for processes
	while( (waitpid(-1, NULL, WNOHANG)) > 0 ){
	}
}

/*
 * A function to run a give array of arguments
 *
 * @param argv A pointer to the array of arguments
 */
void runCommand(char **argv) {
	int ret = execvp(*argv, argv);
	if (ret == -1) {
		printf("ERROR: Command not found\n");
		exit(1);
	}
	exit(2);
//	return;
}

/*
 * A function to determine if the command is executing a command
 * from history
 *
 * @param str A pointer to the array of arguments
 *
 * @return Returns -1 if not running a command from history, otherwise returns
 * the number of the command
 */
int isHistoryCmd( char **str){
	int num = 0;
	if( str[0][0] != ('!')){
		return -1;
	} else {
		num = strtol( str[0]+1, NULL, 10);
		return num;
	}
}

/*
 * A function to check if the command entered was exit
 *
 * @param argv A pointer to the argument array
 */
void terminate(char **argv) {
	if(strcmp(argv[0], "exit") == 0 ) {
		printf("Exiting Tiny Torero Shell\n");
		exit(0);
	}
}

/*
 * A function to handle the given arguments
 *
 * @param argv A pointer to the array of arguments
 * @param background_flag A pointer to the flag for background processes
 * @param chdir A pointer to the flag indicating we are changing directories
 * @param parse_again A pointer to the flag indicating repeat parsing
 * @param history A pointer to the flag indicating we are running the history
 * command
 *
 */
void readArgs(char **argv, Flags *flags) {
	int ret = 0;
	char *cmd_string = NULL;
	if((argv[1] == NULL) && ((ret = isHistoryCmd(argv)) != -1) ){
		 cmd_string = check_history(ret);
		 if( cmd_string == NULL){
			 printf("Command not found in history\n");
		 }
		 else {
			flags->background_flag = parseArguments( cmd_string, argv);
			if( strcmp( argv[0], "cd") == 0 ){
			   flags->parse_again = 1;
		  	}	   
		 }
	} else if( strcmp( argv[0], "cd" ) == 0){
		flags->chdir_flag = 1;
		if (argv[1] == NULL) {
		   chdir(getenv("HOME"));
		   return;
		} else {
	  		char cwd[4000];
			getcwd( cwd, sizeof(cwd));
			if( chdir( addStrings( cwd, argv[1]) ) == -1 ){
				printf("Invalid directory\n");
			}
		}
 	} else if( strcmp( argv[0], "history") == 0) {
		flags->history = 1;
		print_history();
	}		
}

/*
 * A function to combine two strings
 *
 * @param str1 The first string to be combined
 * @param str2 The second string to be combined
 *
 * @return A pointer to the string that str1 and str2 were combined into
 */
char *addStrings( char *str1, char *str2){
	char *str3 = malloc(1 + strlen(str1) + strlen(str2) );
	strcpy( str3, str1 );
	strcat( str3, str2 );
	return str3;
}

/*
 * A function for reseting a number of pointers to zero
 *
 * @param chdir_flag A pointer to a flag indicating we are changing
 * directories
 * @param history A pointer a flag indicating we are runnning the history
 * command
 * @param parse_again A pointer to a commmand indicating we are going to have
 * to parse again
 */
void resetPointers(Flags *flags){
	flags->chdir_flag = 0;
	flags->history = 0;
	flags->parse_again = 0;
}

/*
 * A function to initialize the flags to zero
 *
 * @param flags A pointer to the flags struct
 */
void initFlags(Flags *flags){
	flags->background_flag = 0;
	flags->chdir_flag = 0;
	flags->history = 0;
	flags->parse_again = 0;
}	
