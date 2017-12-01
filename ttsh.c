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



int main() { 
	signal(SIGUSR1, reapHandler);
	int background_flag = 0;
	char *argv[MAXARGS];
	struct sigaction sa;
	sa.sa_handler = reapHandler;
	sa.sa_flags = SA_NOCLDSTOP;
	sigaction(SIGCHLD, &sa, NULL);

	while(1) {
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
		if(argv[0] == NULL){ continue; }

		//Exit
		if(strcmp(argv[0], "exit") == 0 ) {
			printf("Exiting Tiny Torero Shell\n");
			exit(0);
		}

		// (4) Call a function that will determine how to execute the command
		// that the user entered, and then execute it
		int child_pid = fork();
		int ret;
		if (child_pid == 0) {
			if( strcmp(argv[0], "history") == 0){
				print_history();
			} else {
				if( (ret = execvp( *argv, argv)) == -1){
					printf("ERROR: Command not found\n");
				}
			}
		}
		else {
			if(!background_flag) {
				ret = waitpid(child_pid, NULL, 0);
				if( ret == -1) 
					printf("Error: Child not reaped properly\n");
			}
		}
	}
	return 0;
}

void reapHandler() {
	int ret = waitpid(-1, NULL, WNOHANG);
	ret+=0;
}
